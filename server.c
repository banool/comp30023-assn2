// Daniel Porteous porteousd

// Started with the server-2 code from Sockets/TCP2/

#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "game.h"
#include "logging.h"

#define LISTEN_QUEUE 5
#define POLL_INTERVAL 500

void sigint_handler(int dummy);
void end_execution(StateInfo *state_info);

// These are used in the other files, hence defined globally.
// Should be ok in respect to good practice, since these variables all relate
// to execution across the entire life of the server process.
pthread_mutex_t lock;
FILE *f;
int num_connections = 0;
int num_wins = 0;

int main (int argc, char *argv[])
{
	// Allowing server to gracefully handle SIGINT.
	signal(SIGINT, sigint_handler);

	// Making a buffer for the log file and setting it to null.
    char log_buf[LOG_MSG_LEN];
    memset(log_buf, '\0', LOG_MSG_LEN);

    // Stuff for making, binding and then listening on the socket.
	struct sockaddr_in server, client;
	char *host;
	socklen_t len;
	int s, new_s, count, server_port;

	// Setting the correct code to NULL. This will be set for the whole
	// server execution if it was passed as an arg.
	char *correct = NULL;

	// Code for reading in the port number (and code if supplied).
	if (argc == 2) {
		server_port = atoi(argv[1]);
	} else
	if (argc == 3) {
		server_port = atoi(argv[1]);
		correct = argv[2];
	}
	else {
		fprintf(stderr, "Usage: %s [port_number] <code>\n", argv[0]);
		exit(1);
	}

	// Initialising the file pointer for writing to log.
	f = fopen("log.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	// Setting up the mutex log for writing to log / collecting stats.
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        exit(1);
    }

	printf("Server port %i\n",server_port);
	
	/* Building data structures for sockets */
	/* Identify two end points; one for the server and the other for the client 
	when it connects to the server socket */
	memset (&server, 0, sizeof (server));
	memset (&client, 0, sizeof (client));
	
	/* 
	** Server socket initializations.
	** AF_INET: specifies the connection to Internet. In our example we use 
	** TCP port 5431 as the known server port; Any client needs to connect to
	** this port. INADDR_ANY specifies the server is willing to accept 
	** connections on any of the local host's IP addresses.
	*/
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons (server_port); 

	// Preliminary server steps: Setup: creation of passive open socket.
	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
      perror ("Error creating socket");
      exit (1);
	}
	printf("Socket descriptor:  %d\n", s);

	// Bind the socket to local address.
	if (bind (s, (struct sockaddr *) &server, sizeof (server)) < 0)
	{
      perror ("Error in binding to the specified port");
      exit (1);
	}
	printf("sock:  family = %d\n", server.sin_family);
	printf("       saddr  = %d\n", server.sin_addr.s_addr);
	printf("       port   = %d\n", ntohs(server.sin_port));

	// Sets the maximum number of pending connections to be allowed.
	if (listen (s, LISTEN_QUEUE) < 0)
	{
        perror("listen() failed with error");
        exit(1);
	}
	else
	{
		printf("Listening on port %d...\n", ntohs(server.sin_port));
	}

	// Creating the struct that will keep track of all the instances.
	StateInfo *state_info = create_state_info_struct(MAX_PLAYERS);

	// Creating the struct for file descriptors to be polled.
	struct pollfd poll_list[1];
	poll_list[0].fd = s;
	poll_list[0].events = POLLIN|POLLPRI;

	// The return value of poll, which runs each POLL_INTERVAL milliseconds.
	int poll_res;

	// Main server loop.
	while (1) {
		len=sizeof(client);

		/*
		** We use the poll() function outside of the accept so that we can
		** properly handle incoming signal interrupts. accept() is a blocking
		** function and it wouldn't process the SIGINT until a connection
		** had been accepted. poll() checks if there is anything to be read on
		** the socket first (every POLL_INTERVAL) before running the accept().
		**
		** A fork could also have been used to catch the SIGINT.
		** The polling solution however gives you a greater degree of fine tuned
		** control both due to the non-blocking nature and the programmer
		** defined checking interval, which also reduces computational expense.
		**
		** poll_res >= 0: Something ready to be read on the target fd/socket.
		** poll_res == 0: Nothing ready to be read on the target fd/socket.
		** poll_res <= 0: An error occurred, hopefully just because the SIGINT
		**                interrupted the poll. We catch this and gracefully
		**                shutdown the server.
		*/
		poll_res = poll(poll_list, 1, POLL_INTERVAL);

		// This block runs if there is something to be read on the socket.
		if (poll_res > 0) {
			// Accepting the new connection and assigning the socket number.
			if ((new_s = accept(s, (struct sockaddr *) &client, &len)) < 0)
			{
				printf("errno = %d, new_s = %d\n", errno, new_s);
				perror ("Accept failed");
				exit (1);
			}
			else
			{
				// Preparing ip4 for the ipv4 address.
	    		char ip4[INET_ADDRSTRLEN];
	    		inet_ntop(AF_INET,&(client.sin_addr), ip4, INET_ADDRSTRLEN);

	    		// Creating the individual game instance.
	    		if (create_game(new_s, ip4, correct, state_info) < 0){
	                sprintf(log_buf, "Max players (%d) reached.\n \
Connection from %s rejected.\n", MAX_PLAYERS, ip4);
	                write_log(log_buf);
	            } else {
	                sprintf(log_buf, "(%s)(%d) Client connected.\n", ip4,new_s);
	                write_log(log_buf);
	                num_connections += 1;
	            }
	            memset(ip4, '\0', INET_ADDRSTRLEN);
			}
		} else if (poll_res == 0) {
			continue;
		} else {
			// We watch for SIGILL and not SIGINT because the SIGINT causes
			// poll to throw a SIGILL (system call interrupted).
			if (errno == SIGILL) {
				break;
			}
			// Terminating after getting an unexpected error.
			fprintf(stderr, "errno = %d\n", errno);
			perror ("Polling failed");
			exit (1);		
		}
	}

	// We get to this block after receiving the SIGINT. Gracefully exiting.
	end_execution(state_info);
	close(s);
	fclose(f);

	return 1;
}

// Just breaks the while loop in main and returns control to the block after it.
void sigint_handler(int dummy) {
	// This moves the next printed messaged to the next line, away from ^C
    printf("\n");
}

/*
** This function is run when SIGINT is received. Loops through each instance
** in state_info and kills the thread, sends a disconnect message to the client
** and finally frees memory.
*/
void end_execution(StateInfo *state_info) {

    char log_buf[LOG_MSG_LEN];
    sprintf(log_buf, "(0.0.0.0) Server shutting down.\n");
    write_log(log_buf);

    char outgoing[OUTGOING_MSG_LEN];
    sprintf(outgoing, "%dServer shutting down. Sorry!", DEAD);

    fprintf(stderr, "Server terminated.\n");

    sprintf(log_buf, "Performance and resource info:\n");
    write_log(log_buf);

    sprintf(log_buf, "Num connections: %d.\n", num_connections);
    write_log(log_buf);

    sprintf(log_buf, "Num wins: %d\n", num_wins);
	write_log(log_buf);

    sprintf(log_buf, "\nPerformance and resource info:\n");
    write_log(log_buf);

    sprintf(log_buf, "\nNum connections: %d.Num wins: %d\n", num_connections,
    	num_wins);
    write_log(log_buf);

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    printf("help? %d\n", getrusage(RUSAGE_CHILDREN, &usage));

    printf("user CPU time: %d\n", usage.ru_utime.tv_sec);
    printf("system CPU time: %d\n", usage.ru_stime.tv_sec);
    printf("system CPU time: %ld\n", usage.ru_maxrss);

    for (int x = 0; x < state_info->max_size; x++) {
        if (state_info->instances[x] != NULL) {
        	// Kill the threads before sending the shutdown message.
        	// Prevents any rare case where two messages might be sent at once.
        	pthread_cancel(state_info->instances[x]->t);

            send(state_info->instances[x]->s, outgoing, OUTGOING_MSG_LEN, 0);
            close(state_info->instances[x]->s);

            free(state_info->instances[x]);
            state_info->instances[x] = NULL;
            state_info->num_items -= 1;
        }
    }
}
