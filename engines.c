#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "include/functions.h"
#include "include/threads.h"
#include "include/server.h"
#include "include/networker.h"
void bruceforce_serv(int port);

/*
	ENGINES
 Main bruceforce functions
*/
void bruceforce_dictionary(const char *dictpath, const char *hash, char *salt, int THREADS) {
  //Load Dictionaries
  int dictfilecount = 0;
  printf("Loading dictionary from %s\n", dictpath);
  int words = 0;
  char **dictionary = malloc(sizeof(char **));
  load_dictionary(dictpath, &dictionary, &words, &dictfilecount);

  printf("dictionary-word-count=%d, total-files=%d\n", words, dictfilecount);
  //create thread arguments and start threads
  dict_args **threadargs = dictcreate_threadargs(THREADS, salt, hash, words, dictionary);
  pthread_t threads[THREADS - 1];
  for (int i = 0; i < THREADS; i++)
  {
    printf("Starting thread-id=%i, start-from=%i, count=%i\n", i, threadargs[i]->segment_from, threadargs[i]->segment_count);
    pthread_create(&threads[i], NULL, dictionaryThreadCallback, threadargs[i]);
  }
  //Wait until complete but show progress
  bool thread_continue = true;
  while (thread_continue)
  {
    sleep(1);
    for (int i = 0; i < THREADS; i++)
    {
      printf("thread-id=%i, thread-progress=%.2f%% -- ", threadargs[i]->id, (threadargs[i]->p_status));
      if (threadargs[i]->stop == true)
        thread_continue = false;
    }
    printf("\n");
  }
  //Sync thread exits
  for (int i = 0; i < THREADS; i++)
  {
    if (!thread_continue)
      threadargs[i]->stop = true;
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < THREADS; i++)
  {
    if (threadargs[i]->password != NULL)
    {
      printf("Thread %i found the password: %s\n", threadargs[i]->id, threadargs[i]->password);
    }
    free(threadargs[i]);
  }
  for (int i = 0; i < words; i++)
  {
    free(dictionary[i]);
  }
  free(dictionary);
  free(threadargs);
}

void bruceforce_bruteforce(const char *hash, const char *salt, int size, int THREADS, int startNum) {
  printf(" *** Starting bruteforce...\n");
  //progress/timer definitions
  struct timeval start, end;
  int p_time, p_wpm;
  gettimeofday(&start, NULL);


  //define the possible characters we'll test
  const char *charactertable = ALPHABET;

  int charactertablesize = strlen(charactertable);
  printf("password-table-size: %i, block-size: %li, total-blocks: %li\n", charactertablesize, MAX_BLOCK_SIZE, totalblocks);
  bruteforce_args bf_args;
  //Start server, and control the blocks
  bruceforce_serv(27950, bf_args);

  /*  divide the table into segments divided by
      number of threads we'll use, and
      the first character in our search algorithm
      will be changed within its segments.
    */
   /*
  bruteforce_args *targs = bruteforcecreate_threadargs(THREADS, salt, hash, size, charactertable, charactertablesize, startNum);
  pthread_t *threads = malloc(sizeof(pthread_t) * (THREADS + 1));
  for (int i = 0; i < THREADS; i++)
  {
    pthread_create(&threads[i], NULL, bruteforceThreadCallback, &targs[i]);
  }


  //Wait until complete but show progress
  bool thread_continue = true;
  long total_words = 0;
  long last_totalwords = 0;
  struct timeval time_last;
  //Start server
  printf("Starting server...\n");
  //let the threads start
  sleep(2);
  printf("[Total time elapsed] [Words per minute] [id wordsize progress count]...\n");
  while (thread_continue)
  {
    sleep(1.5);
    //print time elapsed & ppm (actual numbers)
    gettimeofday(&end, NULL);
    p_time = ((float)timeDifference(&start, &end) / 1000000);
    double time_last_diff = timeDifference(&time_last, &end);
    p_wpm = ((total_words - last_totalwords) * 60) / 1000 / (time_last_diff / 1000000);
    if (p_wpm < 0)
      p_wpm = 0;
    last_totalwords = total_words;
    time_last = end;

    printf("time: %i sec wpm: %ik -- ", p_time, p_wpm);

    //Check flag to stay in loop
    bool all_threads_dead = true;
    total_words = 0;

    for (int i = 0; i < THREADS; i++)
    {
      total_words += targs[i].p_processed;
      all_threads_dead = all_threads_dead & targs[i].stop; //if any thread is dead

      //Calculate the progress
      float a = (float)targs[i].segment_count / targs[i].c_tablesize;
      long d = a * pow(targs[i].c_tablesize + 1, targs[i].depth + 1);

      if (targs[i].stop == false) //Print only threads that is running
        printf("[%i-b>%i:%.2f%%] ",
               targs[i].id,
               targs[i].depth + 1,
               ((float)targs[i].p_processed / d * 100)); //,
      //targs[i].p_processed);

      if (targs[i].stop == true && targs[i].password != NULL)
        thread_continue = false; //If password is found, stop threads
    }
    if (all_threads_dead == true)
      thread_continue = false; //If no more threads are running, break out of progress view
    printf("\n");
  }

  //Sync thread exits
  for (int i = THREADS - 1; i >= 0; i--)
  {
    if (!thread_continue)
      targs[i].stop = true;
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < THREADS; i++)
  {
    if (targs[i].password != NULL)
    {
      printf("Thread %i found the password: %s\n", targs[i].id, targs[i].password);
      free(targs[i].password);
    }
  }
  free(threads);
  free(targs);*/

}

void bruceforce_serv(int port, bruteforce_args **bf_args) {
  
  long MAX_BLOCK_SIZE = pow(charactertablesize,4);
  //Create the blocks we need for processing
  long totalblocks = pow(charactertablesize,size) / MAX_BLOCK_SIZE;
  bf_block *blocks = malloc(sizeof(bf_block) * totalblocks);
  for (int i = 0;i<totalblocks;i++) {
    blocks->index = i;
    blocks->b_size = size;
    blocks->b_progress = 0;
    blocks->hit = 0;
  }
  

  server_args *args = malloc(sizeof(server_args));
  args->portno = port;
  args->bf_args
  pthread_t thread;
  pthread_create(&thread, NULL, createServerCallback, args);
  pthread_join(thread, NULL);
  free(args);
}

void bruceforce_cli(const char *hash, const char *salt, int tx_port, int port, char* remote_host) {
  printf("Starting Bruceforce Network Client threads, hash: %s salt: %s\n",hash,salt);
  networker_args *args = malloc(sizeof(networker_args));
  args->tx_port = tx_port;
  args->remote_port = port;
  args->remote_host = remote_host;

  pthread_t thread;
  pthread_create(&thread, NULL, createNetworkerCallback, args);
  pthread_join(thread, NULL);
  free(args);
}