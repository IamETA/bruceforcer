#ifndef ENGINES_H_ /* Include guard */
#define ENGINES_H_
void bruceforce_dictionary(const char *dictpath, const char *hash, char *salt, int THREADS);
void bruceforce_bruteforce(const char* hash,const char *salt, int size, int THREADS, int startNum);
void bruceforce_serv(int port);
void bruceforce_cli(const char *hash, const char *salt, int tx_port, int remote_port, char *remote_host);
#endif // ENGINES_H_
