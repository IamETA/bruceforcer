
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include "include/functions.h"
#include "include/server.h"
#include "include/networker.h"


void* createServerCallback(void *args) {
  server_args *context = (server_args*)args;
  printf("Thread starting, using port %i.\n",context->portno);
  runServer(context);
  pthread_exit(NULL);
}
void *createNetworkerCallback(void *args) {
  networker_args *context = (networker_args*)args;
  printf("Bruce Force Network starting (local binding on port: %i).\n",context->tx_port);
  startNetworker(context);
  pthread_exit(NULL);
}
void* bruteforceThreadCallback(void* args) {
  bruteforce_args *context = args;
  printf("thread-id: %i, thread-start-from: %i, count: %i,wordsize: %i\n",context->id,context->segment_from,context->segment_count,context->c_tablesize);
  char *bf_result = bf_hack(args);
  printf("Bruteforce Thread %i exiting. total-words: %li ",context->id,context->p_processed);
  context->stop = true;
  context->password = bf_result;
  if (bf_result==NULL) {
    printf("No results");
  }
  else {
    printf("Found match!");
  }
  printf("\n");
  pthread_exit(NULL);
}

void* dictionaryThreadCallback(void *args)
{
  dict_args *context = args;
  const char *bf_dict_result = bf_dictionary(
    context->dictionary, 
    context->segment_from,
    context->segment_count, 
    context->salt, 
    context->hash,
    &context->p_status,
    &context->stop);
  context->stop = true;
  context->password = bf_dict_result;
  printf("Dictionary Thread %i exited. ",context->id);
  if (bf_dict_result==NULL) {
    printf("No results");
  }
  else {
    printf("Found match!");
  }
  printf("\n");
  pthread_exit(NULL);
}


dict_args **dictcreate_threadargs(int THREADS, char *salt, const char *hash, int words, char **dictionary)
{
  dict_args **threadargs = malloc(sizeof(dict_args *) * (THREADS));
  for (int i = 0; i < THREADS; i++)
  {
    threadargs[i] = malloc(sizeof(dict_args));
    threadargs[i]->p_status = 0;
    threadargs[i]->stop = false;
    threadargs[i]->id = i;
    threadargs[i]->salt = salt;
    threadargs[i]->hash = hash;
    threadargs[i]->segment_from = -(words / THREADS) + ((words / THREADS) * (i + 1));
    threadargs[i]->dictionary = dictionary;
    threadargs[i]->segment_count = (int)(words / THREADS);
    //If the last thread, add the divide REST to it
    if (i == (THREADS - 1))
    {
      threadargs[i]->segment_count += words % (words / THREADS);
    }
  }
  return threadargs;
}

bruteforce_args *bruteforcecreate_threadargs(int THREADS, const char *salt, const char *hash,
                                             int size, const char *charactertable, int charactertablesize, int startNum)
{
  bruteforce_args *targs = malloc(sizeof(bruteforce_args) * (THREADS + 1));
  for (int i = 0; i < THREADS; i++)
  {
    targs[i].password = NULL; //malloc(sizeof(char*));
    targs[i].id = i;
    targs[i].wordsize = size;
    targs[i].c_table = charactertable;
    targs[i].c_tablesize = charactertablesize;
    targs[i].hash = hash;
    targs[i].salt = salt;
    targs[i].startNum = startNum;

    //We need some preinit, or the program will
    //crash during posting the statuses
    targs[i].p_processed = 0;
    targs[i].depth = 0;
    targs[i].p_status = 0;
    targs[i].segment_from = -(charactertablesize / THREADS) + ((charactertablesize / THREADS) * (i + 1));
    targs[i].segment_count = charactertablesize / THREADS;
    targs[i].stop = false;

    //If the last thread, add the divide REST to it
    if (i == (THREADS - 1))
    {
      targs[i].segment_count += charactertablesize % (charactertablesize / THREADS);
    }
  }
  return targs;
}
