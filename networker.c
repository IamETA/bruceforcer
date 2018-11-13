
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "include/networker.h"

int bfn_query_devices(networker_args *context);


void startNetworker(networker_args *context) {
    printf("Querying devices\n");
    int total_devices = bfn_query_devices(context);
    printf("total devices: %i",  total_devices);
}

int bfn_query_devices(networker_args *context) {

  /* get server IP address (no check if input is IP address or DNS name */
    /*
  h = gethostbyname(context->remote_host);
  if(h==NULL) {
    printf("%s: unknown host '%s' \n", context->remote_host, context->remote_host);
    exit(1);
  }

  printf("%s: sending data to '%s' (IP : %s) \n", context->remote_host, h->h_name,
	 inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
*/
  int sd;
  struct sockaddr_in remoteServAddr, cliAddr;
  struct in_addr addr;
  remoteServAddr.sin_family = AF_INET;
  //memcpy((char *) &remoteServAddr.sin_addr.s_addr, 
  //	 h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(context->remote_port);
  inet_aton(context->remote_host,&addr);

  /* socket creation */
  sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) {
    printf("%s: cannot bind port %i\n",context->remote_host, context->remote_port);
    exit(1);
  }
  
  int broadcast = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1) {
          perror("setsockopt (SO_BROADCAST)");
          exit(1);
  }
  
  /* bind any port */
  cliAddr.sin_family = AF_INET;
  cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);
  int rc;
  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) {
    printf("%s: cannot bind port %i\n",context->remote_host, context->remote_port);
    exit(1);
  }
  char * querydata = "\255\255\255\255\0";
  while (1) {
    sleep(3);
    printf("transmitting: %s\n", querydata);
    /* send data */
    rc = sendto(sd, querydata, strlen(querydata)+1, 0,
                (struct sockaddr *)&remoteServAddr,
                sizeof(remoteServAddr));

    if (rc < 0)
    {
        printf("%s: cannot send data %d \n", context->remote_host, context->remote_port);
        close(sd);
        exit(1);
    }
  }

  return 1;    
}

