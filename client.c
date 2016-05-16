// This is the client-2 code from Sockets/TCP2/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

#define CODE_LENGTH 4
#define RECEIVE_LENGTH 32
#define WELCOME_LENGTH 320
#define DEAD '0'

void print_guess_q();

int main(int argc, char * argv[])
{

	char msgtobesent[CODE_LENGTH];
	char welcome[WELCOME_LENGTH];
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

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin))  < 0  )
	{
		perror("Error in connecting to the host");
		close(s);
		exit(1);
	}

	int recv_len;
	char c;

	// Getting welcome message
	recv_len = recv(s, &welcome, WELCOME_LENGTH, 0);
	welcome[recv_len] = '\0';
	printf("%s\n", welcome+1);
	if (welcome[0] == DEAD) {
		return 1;
	}

	print_guess_q();
	while (scanf("%4s", msgtobesent))
	{
		send(s, msgtobesent, CODE_LENGTH, 0);
		recv_len = recv(s, &receive, RECEIVE_LENGTH, 0);
		receive[recv_len] = '\0';

		printf("%s\n", receive+1);
		if (receive[0] == DEAD) {
			break;
		}

		memset(receive, '\0', RECEIVE_LENGTH);

		// While not required as per the spec, this line just
		// clears the input if the user entered more than 4 chars.
		while((c = getchar()) != '\n' && c != EOF);

		print_guess_q();
	}

	send(s, (char*)DEAD, 1, 0);
	close(s);
}

void print_guess_q() {
	printf("Enter guess: ");
}

/*
in terms of not letting either end hang when the other end unexpectedly disconnects, don't you just have an interrupt for ctrl+c that sends something to the other side like "DEAD" or something
still do we need to handle when one side just dies without that, like the server computer loses power or something
who knows
*/