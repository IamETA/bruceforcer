#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "include/functions.h"
#include "include/threads.h"

/*
	ENGINES
 Main bruceforce functions
*/

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
  char **dictionary = malloc(sizeof(char**));
  load_dictionary(dictpath, dictionary, &words,&dictfilecount);
  
  printf("dictionary-word-count=%d, total-files=%d\n", words, dictfilecount);
  //create thread arguments and start threads
  dict_args **threadargs = dictcreate_threadargs(THREADS,salt,hash,words,dictionary);
  pthread_t threads[THREADS-1];
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
  free(threadargs);
}

void bruceforce_bruteforce(const char* hash,const char *salt, int size,int THREADS, int startNum) {
  printf(" *** Stating bruteforce...");
  //Start timer
  clock_t start,end;
  double cpu_time;
  start = clock();
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
    targs[i].password = NULL;//malloc(sizeof(char*));
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
    sleep(1);
    //Show time elapsed
    end = clock();
    cpu_time = ((double) end - start) / CLOCKS_PER_SEC;
    printf("Time elapsed: %.0f sec -- ",cpu_time);
    
    //Check flag to stay in loop
    bool all_threads_dead = true;

    for (int i=0;i<THREADS;i++) {
      all_threads_dead = all_threads_dead & targs[i].stop; //if any thread is dead
      //Calculate the progress
      float a = (float)targs[i].segment_count / targs[i].c_tablesize;
      long d = a * pow(targs[i].c_tablesize+1,targs[i].depth+1);
      if (targs[i].stop == false) //Print only threads that is running
        printf("pid=%i, depth=%i, progress:%.5f, count=%li -- ",
        targs[i].id,
        targs[i].depth+1,
        ((float)targs[i].p_processed / d * 100),
        targs[i].p_processed);
 
      if (targs[i].stop == true && targs[i].password != NULL) thread_continue = false; //If password is found, stop threads
    }
    if(all_threads_dead == true) thread_continue = false; //If no more threads are running, break out of progress view
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
      free(targs[i].password);
    }
  }
  free(threads);
  free(targs);
}
