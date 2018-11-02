
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include "include/functions.h"

void* bruteforceThreadCallback(void* args) {
  bruteforce_args *context = args;
  printf("thread-id: %i, thread-start-from: %i, count: %i,bitsize: %i\n",context->id,context->segment_from,context->segment_count,context->c_tablesize);
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
