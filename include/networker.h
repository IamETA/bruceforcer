#ifndef NETWORKER_H_ /* Include guard */
#define NETWORKER_H_
/* 
 * networker - 
 * reaches out to bruce servers on local network to decrypt password.
 */

typedef struct networker_args
{
  int tx_port;
  const char * hash;
  const char * salt;
  const char * remote_host;
  int remote_port;
  int clients_count;
} networker_args;

void startNetworker(networker_args *context);
#endif // NETWORKER_H_
