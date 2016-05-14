/* Creates a datagram server.  The port 
   number is passed as an argument. 
  
  This server runs forever */

// compile: gcc server_upd.c -o server -lsocket -lnsl

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
   int sock, length, fromlen, n;
   struct sockaddr_in server;
   struct sockaddr_in from;
	
   char buf[1024];

   if (argc < 2) 
   {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);
	
   if (sock < 0) 
   {
	   perror("sock error");
	   exit(0);
   }

   length = sizeof(server);

   bzero(&server,length);
   
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
   {
	   perror("bind error");
	   exit(0);
   }
  
   fromlen = sizeof(struct sockaddr_in);
   
   while (1) 
   {
       n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
	   
       if (n < 0)
       {
		   perror("recvfrom error");
		   exit(0);
       }
	   
       write(1,"Received a datagram: ",21);
       write(1,buf,n);
       
       n = sendto(sock,"Got your message\n",17, 0,
				  (struct sockaddr *)&from,fromlen);
	   
       if (n  < 0) 
       {
		   perror("sendto error");
		   exit(0);
       }
   }

   return 0;
 }
