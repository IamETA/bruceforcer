#ifndef THREADS_H_ /* Include guard */
#define THREADS_H_
void* bruteforceThreadCallback(void *args);
void* dictionaryThreadCallback(void *args);
void* createServerCallback(void *args);

dict_args **dictcreate_threadargs(int THREADS,char *salt, const char* hash, int words,char**dictionary);
bruteforce_args *bruteforcecreate_threadargs(int THREADS,const char *salt, const char* hash,int size,
  const char * charactertable,int charactertablesize,int startNum);
void *createNetworkerCallback(void *args);
#endif // THREADS_H_