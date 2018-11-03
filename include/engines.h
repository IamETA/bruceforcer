#ifndef ENGINES_H_ /* Include guard */
#define ENGINES_H_
void bruceforce_dictionary(const char *dictpath, const char *hash, char *salt, int THREADS);
void bruceforce_bruteforce(const char* hash,const char *salt, int size, int THREADS, int startNum);
#endif // THREADS_H_
