/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "include/server.h"

#define BUFSIZE 1024

//Socket stuff
//From https://www.gnu.org/software/libc/manual/html_node/Creating-a-Socket.html#Creating-a-Socket

void error(char *msg)
{
	perror(msg);
	exit(1);
}


void runServer(server_args *context)
{
	int sockfd;		/* socket */
	int portno;		/* port to listen on */
	unsigned int clientlen;		/* byte size of client's address */
	struct sockaddr_in serveraddr;	/* server's addr */
	struct sockaddr_in clientaddr;	/* client addr */
	struct hostent *hostp;	/* client host info */
	char *buf;		/* message buf */
	char *hostaddrp;	/* dotted decimal host addr string */
	int optval;		/* flag value for setsockopt */
	int n;			/* message byte size */

	portno = context->portno;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	/* setsockopt: Handy debugging trick that lets 
	 * us rerun the server immediately after we kill it; 
	 * otherwise we have to wait about 20 secs. 
	 * Eliminates "ERROR on binding: Address already in use" error. 
	 */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
		   (const void *)&optval, sizeof(int));

	/*
	 * build the server's Internet address
	 */
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);

	/* 
	 * bind: associate the parent socket with a port 
	 */
	if (bind(sockfd, (struct sockaddr *)&serveraddr,
		 sizeof(serveraddr)) < 0)
		error("ERROR on binding");

	printf("BruceForcer listening on port %i\n",context->portno);
	/* 
	 * main loop: wait for a datagram, then echo it
	 */
	clientlen = sizeof(clientaddr);
	while (1) {

		/*
		 * recvfrom: receive a UDP datagram from a client
		 */
		buf = malloc(BUFSIZE);
		n = recvfrom(sockfd, buf, BUFSIZE, 0,
			     (struct sockaddr *)&clientaddr, &clientlen);
		if (n < 0)
			error("ERROR in recvfrom");

		/* 
		 * gethostbyaddr: determine who sent the datagram
		 */
		hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
				      sizeof(clientaddr.sin_addr.s_addr),
				      AF_INET);
		if (hostp == NULL)
			error("ERROR on gethostbyaddr");
		hostaddrp = inet_ntoa(clientaddr.sin_addr);
		if (hostaddrp == NULL)
			error("ERROR on inet_ntoa\n");
		printf("server received %d bytes\n", n); 

		/* 
		 * sendto: echo the input back to the client 
		 */
		n = sendto(sockfd, buf, n, 0,
			   (struct sockaddr *)&clientaddr, clientlen);
		if (n < 0)
			error("ERROR in sendto");
	}
}
