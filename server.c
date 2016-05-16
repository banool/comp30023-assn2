// This is the server-2 code from Sockets/TCP2/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "game.h"
#include "logging.h"
//#include "threads.h"

int main (int argc, char *argv[])
{

	char msg[25];
    char log_buf[LOG_MSG_LEN];
    memset(log_buf, '\0', LOG_MSG_LEN);

	struct sockaddr_in server, client;
	char *host;
	socklen_t len;
	int s, new_s, count,server_port;


	if (argc==2){
		server_port = atoi(argv[1]);
	}
	else {
		fprintf(stderr, "Usage :server portnumber\n");
		exit(1);
	}
	printf("Server port %i\n",server_port);
	/* Building data structures for sockets */
	/* Identify two end points; one for the server and the other for the client 
	when it connects to the server socket */
	memset (&server, 0, sizeof (server));
	memset (&client, 0, sizeof (client));
	/* Server socket initializations */
	/* AF_INET: specifies the connection to Internet. In our example we use 
	TCP port 5431 as the known server port; Any client need to connect to this 
	port;
	INADDR_ANY specifies the server is willing to accept connections on any 
	of the local host's IP addresses. */ 

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons (server_port); 

	/* Preliminary server steps: Setup: creation of passive open socket*/

	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
      perror ("Error creating socket");
      exit (1);
	}
	printf("Socket descriptor:  %d\n", s);

	/* Bind the socket to local address */

	if (bind (s, (struct sockaddr *) &server, sizeof (server)) < 0)
	{
      perror ("Error in binding to the specified port");
      exit (1);
	}
	printf("sock:  family = %d\n", server.sin_family);
	printf("       saddr  = %d\n", server.sin_addr.s_addr);
	printf("       port   = %d\n", ntohs(server.sin_port));

	/* Sets the maximum number of pending connections to be allowed, in our case this number is 10 */

    // TODO magic number care
	if ( listen (s, 5) < 0)
	{
        perror("listen() failed with error");
        exit(1);
	}
	else
	{
		printf("Listening on port %d...\n", ntohs(server.sin_port));
	}
	/* The main loop of the program*/
	/* Wait for connection then receive and print text */

	/* set an alarm to terminate after 1 minutes */
	//diag
	//todo remove
	//alarm(60);

	Instances *instances = create_instances_struct(MAX_PLAYERS);

	while (1) {
		len=sizeof(client);

		if ((new_s = accept (s, (struct sockaddr *) &client, &len)) < 0)
		{
			printf("errno = %d, count =%d, new_s = %d\n", errno, count, new_s);
			perror ("Accept failed");
			exit (1);
		}
		else
		{
    		char ip4[INET_ADDRSTRLEN];
    		inet_ntop(AF_INET,&(client.sin_addr), ip4, INET_ADDRSTRLEN);

    		if (create_game(new_s, ip4, instances) < 0){
                sprintf(log_buf, "Max players (%d) reached. Connection rejected from %s.\n", MAX_PLAYERS, ip4);
                write_log(log_buf);
            } else {
                sprintf(log_buf, "(%s)(%d) Client connected.\n", ip4, new_s);
                write_log(log_buf);
            }
		}
	}
	close(s);

	return 1;
}

/*
void intHandler(int dummy);
void intHandler(int dummy) {
    keep_running = 0;
}
	signal(SIGINT, intHandler);

// Global var ok?
static volatile int keep_running = 1;
*/

