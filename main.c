#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <pthread.h>
#include <sys/time.h>
#include "include/main.h"
#include "include/crypto.h"
#include "include/functions.h"

//So this algorithm is a little funny becuase it is not 
//linear, it will find more complex password faster, if
//the alphabet is composed correctly, and if you are lucky
//with dividing the alphabet in the threads.
//MAX_BF_SIZE is will make the function grow exponentially, 
#define DEFAULT_DICTIONARY_PATH "./dictionaries/" //we'll only use this once
#define ALPHABET "abcdefghikjlmnopqrstuvwxyz";//ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 !#$%&'()*+,-./:;<=>?@[\\]^_`{|}~\"\0";
#define HASH_SALT_SIZE 11
#define SET_THREADS 3
#define MAX_BF_SIZE 4

void* bruteforceThreadCallback(void *args);
void* dictionaryThreadCallback(void *args);
void bruceforce_dictionary(const char *dictpath, const char *hash, char *salt, int THREADS);
void bruceforce_bruteforce(const char* hash,const char *salt, int size, int THREADS);

int main(int argc, char const *argv[])
{
  if (argc < 2)
  {
    printf("Syntax: bruceforce <hash> <optional dictionarypath>\n");
    exit(EXIT_FAILURE);
  }
  const char *hash = argv[1];
  const char *dictpath = DEFAULT_DICTIONARY_PATH;

  //Exctract type and salt from hash
  char *salt = malloc(HASH_SALT_SIZE+1);
  salt[HASH_SALT_SIZE] = '\0';

  strncpy(salt, hash, HASH_SALT_SIZE);
  printf("hash: %s, salt: %s\n", hash, salt);
  if (argc > 2)
  {
    dictpath = argv[2];
  }

  bruceforce_dictionary(dictpath,hash,salt,SET_THREADS);
  bruceforce_bruteforce(hash,salt,MAX_BF_SIZE,SET_THREADS);

  printf("Complete\n.");
  free(salt);

  return 0;
}

dict_args **dictcreate_threadargs(int THREADS,char *salt, const char* hash, int words,char***dictionary) {
  dict_args **threadargs = malloc(sizeof(dict_args*) * (THREADS+1));
  for (int i = 0; i < THREADS; i++)
  {
    threadargs[i]=malloc(sizeof(dict_args));
    threadargs[i]->p_status = 0;
    threadargs[i]->stop = false;
    threadargs[i]->id = i;
    threadargs[i]->salt = salt;
    threadargs[i]->hash = hash;
    threadargs[i]->segment_from = -(words / THREADS) + ((words / THREADS) * (i+1));
    threadargs[i]->dictionary = dictionary;
    threadargs[i]->segment_count = (int)(words / THREADS);
    //If the last thread, add the divide REST to it
    if (i == (THREADS-1)) {
      threadargs[i]->segment_count += words % (words / THREADS);
    }
  }
  return threadargs;
}

void bruceforce_dictionary(const char *dictpath, const char *hash, char *salt, int THREADS) {
  //Load Dictionaries
  int dictfilecount = 0;
  printf("Loading dictionary from %s\n", dictpath);
  int words = 0;
  char **dictionary = NULL;
  load_dictionary(dictpath, &dictionary, &words,&dictfilecount);
  
  printf("dictionary-word-count=%d, total-files=%d\n", words, dictfilecount);
  //create thread arguments and start threads
  dict_args **threadargs = dictcreate_threadargs(THREADS,salt,hash,words,&dictionary);
  pthread_t threads[THREADS+1];
  for (int i = 0; i < THREADS; i++)
  {
    printf("Starting thread-id=%i, start-from=%i, count=%i\n",i,threadargs[i]->segment_from,threadargs[i]->segment_count);
    pthread_create(&threads[i], NULL, dictionaryThreadCallback, threadargs[i]);
  }
  //Wait until complete but show progress
  bool thread_continue = true;
  while (thread_continue) {
    sleep(1);
    for (int i=0;i<THREADS;i++) {
      printf("thread-id=%i, thread-progress=%.2f%% -- ",threadargs[i]->id,(threadargs[i]->p_status));
      if (threadargs[i]->stop == true) thread_continue = false;
    }
    printf("\n");

  }
  //Sync thread exits
  for (int i=0;i<THREADS;i++) {
    if (!thread_continue) threadargs[i]->stop = true;
    pthread_join(threads[i],NULL);
  }

  for (int i=0;i<THREADS;i++) {
    if (threadargs[i]->password != NULL) {
      printf("Thread %i found the password: %s\n",threadargs[i]->id,threadargs[i]->password);
    }
  }
  for(int i=0;i<words;i++) {
    free(dictionary[i]);
  }
  free(dictionary);

}

void bruceforce_bruteforce(const char* hash,const char *salt, int size,int THREADS) {
  //all possible characters we'll test
  const char *charactertable = ALPHABET;
  int charactertablesize = strlen(charactertable);
  printf("password-table-size: %i\n",charactertablesize);

  /*  divide the table into segments divided by
      number of threads we'll use, and
      the first character in our search algorithm
      will be changed within its segments.
    */
  bruteforce_args *targs = malloc(sizeof(bruteforce_args)*(THREADS+1));
  pthread_t *threads = malloc(sizeof(pthread_t)*(THREADS+1));
  for(int i=0;i<THREADS;i++){
    //targs[i] = malloc(sizeof(bruteforce_args));
    targs[i].password = malloc(sizeof(char*));
    targs[i].id = i;
    targs[i].wordsize = size;
    targs[i].c_table = charactertable;
    targs[i].c_tablesize = charactertablesize;
    targs[i].hash = hash;
    targs[i].salt = salt;
    targs[i].p_processed = 0;
    targs[i].p_status = 0;
    targs[i].segment_from = -(charactertablesize / THREADS) + ((charactertablesize / THREADS) * (i+1));
    targs[i].segment_count = charactertablesize/THREADS;
    targs[i].stop = false;

    //If the last thread, add the divide REST to it
    if (i == (THREADS-1)) {
      targs[i].segment_count += charactertablesize % (charactertablesize / THREADS);
    }
    
    pthread_create(&threads[i],NULL,bruteforceThreadCallback,&targs[i]);
  }
  
  //Wait until complete but show progress
  bool thread_continue = true;
  while (thread_continue) {
    sleep(3);
    for (int i=0;i<THREADS;i++) {
      printf("thread-id=%i, segment=%i/%i, segment-progress=%.2f%%, words-processed:%li -- ",
        targs[i].id,targs[i].activesegment,targs[i].segment_count,targs[i].p_status,targs[i].p_processed);
      if (targs[i].stop == true) thread_continue = false;
    }
    printf("\n");
  }

  //Sync thread exits
  for (int i=THREADS-1;i>=0;i--) {
    if (!thread_continue) targs[i].stop = true;
    pthread_join(threads[i],NULL);
  }

  for (int i=0;i<THREADS;i++) {
    if (targs[i].password != NULL) {
      printf("Thread %i found the password: %s\n",targs[i].id,targs[i].password);
      exit(EXIT_SUCCESS);
    }
  }
}
void* bruteforceThreadCallback(void* args) {
  bruteforce_args *context = args;
  printf("thread-id: %i, thread-start-from: %i, count: %i,bitsize: %i\n",context->id,context->segment_from,context->segment_count,context->c_tablesize);
  char *bf_result = bf_hack(args);
  printf("Bruteforce Thread %i exiting. ",context->id);
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
