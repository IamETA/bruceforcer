#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <sys/time.h>
#include "include/main.h"
#include "include/crypto.h"
#include "include/functions.h"
#include "include/engines.h"

//Defaults
#define DEFAULT_DICTIONARY_PATH "./dictionaries/" //we'll only use this once
#define HASH_SALT_SIZE 11


void printhelp() {
  printf("Syntax: bruceforce -h '<hash>' -d '<optional dictionarypath>' -t <number of threads> -b <size of bytes>\n");
  printf("Extra flags: `-s` to skip directory check.\n\n");
}
void printlogo() {
  printf("********************************** \n");
  printf("***        Bruce forcer        *** \n");
  printf("***   Written by Erik Alvarez  *** \n");
  printf("***         01.11.2018         *** \n");
  printf("********************************** \n");
}
int main(int argc, char *argv[])
{
  printlogo();

  int byteSize = 4;
  int threadCount = 4;
  int skipdictionary = 0;
  const char *hash;
  const char *dictpath = DEFAULT_DICTIONARY_PATH;
  int c;
  while ((c = getopt (argc, argv, "htdsb")) != -1)
    switch (c)
      {
      case 't':
	      printf("set-threads:%s ",argv[optind]);
        threadCount = atoi(argv[optind]);
        break;
      case 'h':
        printf("set-hash:%s ",argv[optind]);
        hash = argv[optind];
        break; 
      case 'd':
        printf("set-directory:%s ", argv[optind]);
        dictpath = argv[optind];
        break;
      case 's':
        printf("skip-dictionary ");
        skipdictionary = 1;
        break;
      case 'b':
        printf("set-max-bytes: %s ",argv[optind]);
        byteSize = atoi(argv[optind]);
        break;
      default:
        printhelp();
        exit(EXIT_FAILURE);
      }

  if (argc < 2) {
    printhelp();
    exit(EXIT_FAILURE);
  }
  printf("\n");
/*
  for (int index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
*/

  //Exctract type and salt from hash
  char *salt = malloc(HASH_SALT_SIZE+1);
  salt[HASH_SALT_SIZE] = '\0';
  strncpy(salt, hash, HASH_SALT_SIZE);
  printf("hash: %s, salt: %s, threads:%i, max-byte-size: %i\n", hash, salt,threadCount,byteSize);
  if (skipdictionary==0)
    bruceforce_dictionary(dictpath,hash,salt,threadCount);
  bruceforce_bruteforce(hash,salt,byteSize,threadCount);

  printf("Complete\n.");
  free(salt);

  return 0;
}
