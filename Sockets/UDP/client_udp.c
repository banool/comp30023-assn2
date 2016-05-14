/* UDP client in the internet domain */

// compile: gcc client_upd -o client -lsocket -lnsl

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
   int sock, length, n;
   struct sockaddr_in server, from;
   struct hostent *hp;
   char buffer[256];
   
   if (argc != 3) 
   { 
	   printf("Usage: server port\n");
	   exit(1);
   }
   
   sock= socket(AF_INET, SOCK_DGRAM, 0);
	
   if (sock < 0) 
   {
	   perror("sock error");
	   exit(0); 
   }

   server.sin_family = AF_INET;
	
   hp = gethostbyname(argv[1]);
   
   if (hp==0)
   {
	   perror("hp error");
	   exit(0);
   }


   bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	
   server.sin_port = htons(atoi(argv[2]));
   
   length=sizeof(struct sockaddr_in);
   
   printf("Please enter the message: ");
   
   bzero(buffer,256);
   
   fgets(buffer,255,stdin);
   
   n=sendto(sock,buffer, strlen(buffer),0,&server,length);
   
   if (n < 0) 
   {
	   perror("sendto error");
	   exit(0);
   }

   n = recvfrom(sock,buffer,256,0,&from, &length);
	
   if (n < 0) 
   {
	   perror("recvfrom error");
	   exit(0);
   }
	
   write(1,"Got an ack: ",12);
   write(1,buffer,n);
	
   close(sock);
   
   return 0;	
}
