// This is the server-2 code from Sockets/TCP2/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "game.h"
//#include "threads.h"

int main (int argc, char *argv[])
{

	char msg[25];

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
	printf("Socket descriptor:  %ld\n", s);

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

	Threads *threads = create_threads_struct(MAX_PLAYERS);

	count = 0;
	while (1)
	{
		++count;
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
			printf("connection accepted from client %s\n",ip4);
			create_game(new_s, threads);
		}
	}
	close(s);

	return 1;
}


