// Daniel Porteous porteousd

// Started with the client-2 code from Sockets/TCP2/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define CODE_LENGTH 4
#define RECEIVE_LENGTH 32
#define WELCOME_LENGTH 320
#define DEAD '0'

void print_guess_q();

int main(int argc, char * argv[])
{
	// Buffers for various messages between the server and client.
	char msgtobesent[CODE_LENGTH];
	char welcome[WELCOME_LENGTH];
	char receive[RECEIVE_LENGTH];

	// Stuff for making, binding and then listening on the socket.
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	int len, numberofbytes, s, server_port, counter;

	if (argc == 3) {
		host = argv[1];
		server_port = atoi(argv[2]);
	}
	else {
		fprintf(stderr, "Usage: %s [host] [port_number]\n", argv[0]);
		exit(1);
	}

	/* 
	** Translate host name into peer's IP address.
	** This is the name translation service by the operating system.
	*/
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "Unknown host %s  \n",host);
		exit(1);
	}

	// Building data structures for socket.
	bzero( (char *)&sin,sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port =htons(server_port);

	// Preliminary steps: Setup: creation of active open socket.
	if ( (s = socket(AF_INET, SOCK_STREAM, 0 )) < 0 )
	{
		perror("Error in creating a socket to the server");
		exit(1);
	}

	// Trying to connect to the host.
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin))  < 0  )
	{
		perror("Error in connecting to the host");
		close(s);
		exit(1);
	}

	int recv_len;
	char c;

	// Getting welcome message.
	recv_len = recv(s, &welcome, WELCOME_LENGTH, 0);
	printf("%.*s\n", recv_len-1, welcome+1);

	// Killing execution if the server sent the DEAD char.
	if (welcome[0] == DEAD) {
		return 1;
	}

	/*
	** Main loop for the client. Waits for the 4 characters to be sent to
	** the server. Once the user enters the 4 chars for the guess it is sent
	** to the server. Once we receive the response from the server, we check
	** to see if the message has a DEAD char at the start. If so, break from
	** the loop, close the socket and quit. Otherwise, we reset the receive
	** buffer, clear the input buffer and go around again.
	*/
	print_guess_q();
	while (scanf("%4s", msgtobesent))
	{
		send(s, msgtobesent, CODE_LENGTH, 0);
		recv_len = recv(s, &receive, RECEIVE_LENGTH, 0);

		// Printing the output and breaking if the server sent the DEAD char.
		printf("%.*s\n", recv_len-1, receive+1);
		if (receive[0] == DEAD) {
			break;
		}

		memset(receive, '\0', RECEIVE_LENGTH);

		// While not required as per the spec, this line just
		// clears the input if the user entered more than 4 chars.
		while((c = getchar()) != '\n' && c != EOF);

		print_guess_q();
	}
	close(s);
}

void print_guess_q() {
	printf("Enter guess: ");
}
