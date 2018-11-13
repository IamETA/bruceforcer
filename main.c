#include <unistd.h>
int getopt(int argc, char *const argv[],
           const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <ctype.h>
#include <sys/time.h>
#include "include/main.h"
#include "include/crypto.h"
#include "include/functions.h"
#include "include/engines.h"
#include "include/networker.h"

//Defaults
#define DEFAULT_DICTIONARY_PATH "./dictionaries/" //we'll only use this once
#define HASH_SALT_SIZE 11

void printhelp()
{
  printf("Syntax: bruceforce -h '<hash>' -d '<optional dictionarypath>' -t <number of threads> -s <size of bytes>\n");
  printf("Extra flags: `-b` to skip directory check.\n");
  printf("	`-n N` start on wordlength\n");
}
void printlogo()
{
  printf("********************************** \n");
  printf("***        Bruce forcer        *** \n");
  printf("***          C Exam            *** \n");
  printf("***         01.11.2018         *** \n");
  printf("********************************** \n");
}
clyps *cdata;
int main(int argc, char *argv[])
{
  printlogo();
  char *host = NULL;
  int byteSize = 8;
  int threadCount = 4;
  int skipdictionary = 0;
  int startNum = 2; //2 Minimum
  const char *hash = NULL;
  const char *dictpath = DEFAULT_DICTIONARY_PATH;
  int c;
  while ((c = getopt(argc, argv, "htdsbnc")) != -1)
    switch (c)
    {
    case 't':
      printf("set-threads:%s ", argv[optind]);
      threadCount = atoi(argv[optind]);
      break;
    case 'h':
      printf("set-hash:%s ", argv[optind]);
      hash = argv[optind];
      break;
    case 'd':
      printf("set-directory:%s ", argv[optind]);
      dictpath = argv[optind];
      break;
    case 'b':
      printf("skip-dictionary ");
      skipdictionary = 1;
      break;
    case 's':
      printf("set-max-bytes: %s ", argv[optind]);
      byteSize = atoi(argv[optind]);
      break;
    case 'n':
      printf("start-on: %s", argv[optind]);
      startNum = atoi(argv[optind]);
      break;
    case 'c':
      printf("connect-to: %s", argv[optind]);
      host = argv[optind];
      break;
    default:
      printhelp();
      exit(EXIT_FAILURE);
    }

  //Validation
  if (argc < 2)
  {
    printhelp();
    exit(EXIT_FAILURE);
  }
  else if (hash == NULL)
  {
    printhelp();
    hash = argv[optind];
  }

  if (startNum < 2)
  {
    startNum = 2;
  }
  printf("\n");

  //Exctract type and salt from hash
  char *salt = malloc(HASH_SALT_SIZE + 1);
  salt[HASH_SALT_SIZE] = '\0';
  strncpy(salt, hash, HASH_SALT_SIZE);
  printf("hash: %s, salt: %s, threads:%i, max-byte-size: %i\n", hash, salt, threadCount, byteSize);
  
  if (host != NULL) {
    //Connect to bruceforce client and help processing
    bruceforce_cli(hash,salt, (rand() % 5000) + 56500, 27950, host);
  }
  else {
    if (skipdictionary == 0)
    {
      bruceforce_dictionary(dictpath, hash, salt, threadCount);
      printf("Press ANY key to continue");
      getchar();
    }
    bruceforce_bruteforce(hash, salt, byteSize, threadCount, startNum);
  }
  
  free(salt);

  return 0;
}
