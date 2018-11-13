#ifndef SERVER_H_ /* Include guard */
#define SERVER_H_

typedef struct server_args 
{
	int portno;
	int status;
	char *hash;
	char *salt;
} server_args;
void runServer(server_args *context);
#endif // SERVER_H_
