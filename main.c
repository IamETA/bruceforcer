#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/main.h"
#include <sys/time.h>
#include "include/crypto.h"
#include "include/functions.h"

#define HASH_ENCRYPTION_TYPE_START 1
#define HASH_ENCRYPTION_TYPE_SIZE 1
#define HASH_ENCRYPTION_SALT_START 3
#define HASH_ENCRYPTION_SALT_SIZE 8

int main(int argc, char const *argv[])
{
  if (argc < 2)
  {
    printf("Syntax: bruceforce <hashedvalue>\n");
    exit(EXIT_FAILURE);
  }
  const char *hashedvalue = argv[1];
  //Exctract type and salt from hashedvalue
  char *p_type = malloc(HASH_ENCRYPTION_TYPE_SIZE + 1);
  char *p_salt = malloc(HASH_ENCRYPTION_SALT_SIZE + 1);
  strncpy(p_type, hashedvalue + HASH_ENCRYPTION_TYPE_START, HASH_ENCRYPTION_TYPE_SIZE);
  strncpy(p_salt, hashedvalue + HASH_ENCRYPTION_SALT_START, HASH_ENCRYPTION_SALT_SIZE);

  const char *dictpath = "./dictionary";
  int dictcount;
  //Load Dictionaries
  char **dictionaries;
  getfiles(&dictionaries,&dictcount,dictpath);
  
  int words = 0;
  char **dictionary = NULL;// = malloc(sizeof(char**));
  load_dictionary(dictionaries[0],&dictionary,&words);

  //Validate hashedvalue & dictionarfile
  
  
  printf("hash: %s\n", hashedvalue);
  printf("salt: %s, type: %s\n", p_salt, p_type);
  
  printf("dictionary-word-count=%d,dictsize=%ld",words,sizeof(dictionary));
  free(p_type);
  free(p_salt);

  return 0;
}
/*
void bf_dictionary(char ***dictionary, char *passwd, char *salt)
{
}
void bf_normal()
{
}*/