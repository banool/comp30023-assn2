// This is the client-2 code from Sockets/TCP2/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define CODE_LENGTH 4
#define RECEIVE_LENGTH 30
#define WELCOME_LENGTH 320

void print_guess_q();

int main(int argc, char * argv[])
{

	char msgtobesent[CODE_LENGTH];
	char receive[RECEIVE_LENGTH];

	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	int len, numberofbytes, s, server_port, counter;

	if(argc==3){
		host = argv[1];
		server_port = atoi(argv[2]);
	}
	else {
		fprintf(stderr, "Usage :client host server_port\n");
		exit(1);
	}

	/* translate host name into peer's IP address ; This is name translation service by the operating system */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "Unknown host %s  \n",host);
		exit(1);
	}
	/* Building data structures for socket */

	bzero( (char *)&sin,sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port =htons(server_port);

	/* Active open */
	/* Preliminary steps: Setup: creation of active open socket*/

	if ( (s = socket(AF_INET, SOCK_STREAM, 0 )) < 0 )
	{
		perror("Error in creating a socket to the server");
		exit(1);
	}

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin))  < 0  )
	{
		perror("Error in connecting to the host");
		close(s);
		exit(1);
	}

	// Getting welcome message
	recv(s, &receive, WELCOME_LENGTH, 0);
	printf("%s\n", receive);

	print_guess_q();
	while(scanf("%4s", msgtobesent))
	{
		send(s, msgtobesent, CODE_LENGTH, 0);
		//fflush(stdin);
		recv(s,&receive,RECEIVE_LENGTH,0);

		printf("%s\n", receive);

		print_guess_q();
	}
	close(s);
}

void print_guess_q() {
	printf("Enter guess: ");
}