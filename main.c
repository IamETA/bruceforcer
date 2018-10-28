
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


#define DEFAULT_DICTIONARY_PATH "./dictionaries/" //we'll only use this once
#define HASH_SALT_SIZE 11
#define THREADS 4


void* RunThread(void *args)
{
  dict_args *context = args;

  //printf("Thread %i running\n",context->id);
  //printf("First word: %s\n",(*context->dictionary)[context->startfrom]);
  //Validate hashedvalue & dictionarfile
  char *bf_dict_result = bf_dictionary(
    context->dictionary, 
    context->startfrom,
    context->count, 
    context->salt, 
    context->hash,
    &context->p_status,
    &context->stop);

  context->stop = true;
  //strcpy(context->password,bf_dict_result);
  context->password = bf_dict_result;
  printf("Thread %i exited. ",context->id);
  if (bf_dict_result==NULL) {
    printf("No results");
  }
  else {
    printf("Found match!");
  }
  printf("\n");
  pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{

  if (argc < 2)
  {
    printf("Syntax: bruceforce <hashedvalue> <optional dictionarypath>\n");
    exit(EXIT_FAILURE);
  }
  const char *hashedvalue = argv[1];
  const char *dictpath = DEFAULT_DICTIONARY_PATH;

  //Exctract type and salt from hashedvalue
  char *p_type_salt = malloc(HASH_SALT_SIZE+1);
  p_type_salt[HASH_SALT_SIZE] = '\0';
  strncpy(p_type_salt, hashedvalue, HASH_SALT_SIZE);
  printf("hash: %s, salt: %s\n", hashedvalue, p_type_salt);
  if (argc > 2)
  {
    dictpath = argv[2];
  }

  //Load Dictionaries
  int dictfilecount = 0;
  char **dictionaries = NULL;
  printf("Reading dictionary folder: %s\n", dictpath);
  getfiles(&dictionaries, &dictfilecount, dictpath);

  int words = 0;
  char **dictionary = NULL; // = malloc(sizeof(char**));

  for (int i = 0; i < dictfilecount; i++)
  {
    printf("Loading dictionary %s\n", dictionaries[i]);
    char *dictionaryfilepath = concat(dictpath, dictionaries[i]);
    load_dictionary(dictionaryfilepath, &dictionary, &words);
    free(dictionaryfilepath);
  }
  printf("dictionary-word-count=%d, total-files=%d\n", words, dictfilecount);

  //create thread arguments and start threads
  dict_args threadargs[THREADS];// = malloc(sizeof(dict_args) * THREADS);
  pthread_t threads[THREADS+1];
  for (int i = 0; i < THREADS; i++)
  {
    //allocate place to save thread status on the heap
    //threadargs[i].p_status = malloc(sizeof(int));
    
    threadargs[i].p_status = 0;
    //threadargs[i].stop = malloc(sizeof(bool));
    threadargs[i].stop = false;
    
    threadargs[i].id = i;
    threadargs[i].salt = p_type_salt;
    threadargs[i].hash = hashedvalue;
    threadargs[i].startfrom = ((words / THREADS) * (i+1)) - 1;
    threadargs[i].dictionary = &dictionary;
    threadargs[i].count = (int)(words / THREADS);
    //If the last thread, add the divide REST to it
    if (i == (THREADS-1)) {
      threadargs[i].count += words % (words / THREADS);
    }

    printf("Starting thread-id=%i, start-from=%i, count=%i\n",i,threadargs[i].startfrom,threadargs[i].count);
    pthread_create(&threads[i], NULL, RunThread, &threadargs[i]);
  }
  //Wait until complete but show progress
  bool thread_continue = true;
  while (thread_continue) {
    sleep(1);
    for (int i=0;i<THREADS;i++) {
      printf("thread-id=%i, thread-progress=%.2f%% -- ",threadargs[i].id,(threadargs[i].p_status));
      if (threadargs[i].stop == true) thread_continue = false;
    }
    printf("\n");

  }
  //Sync thread exits
  for (int i=0;i<THREADS;i++) {
    if (!thread_continue) threadargs[i].stop = true;
    pthread_join(threads[i],NULL);
  }

  for (int i=0;i<THREADS;i++) {
    if (threadargs[i].password != NULL) {
      printf("Thread %i found the password: %s\n",threadargs[i].id,threadargs[i].password);
    }
  }

  printf("Complete\n.");
  free(p_type_salt);
  for (int i = 0;i<dictfilecount;i++) {
    free(dictionaries[i]);
  }
  free(dictionaries);
  for(int i=0;i<words;i++) {
    free(dictionary[i]);
  }
  free(dictionary);

  return 0;
}

/*
void bf_normal()
{
}*/