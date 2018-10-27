#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/main.h"
#include <sys/time.h>
#include "include/crypto.h"

#define HASH_ENCRYPTION_TYPE_START 1
#define HASH_ENCRYPTION_TYPE_SIZE 1
#define HASH_ENCRYPTION_SALT_START 3
#define HASH_ENCRYPTION_SALT_SIZE 8

int main(int argc, char const *argv[])
{
  if (argc < 2)
  {
    printf("Syntax: bruceforce <hashedvalue> <dictionary_file>\n");
    exit(EXIT_FAILURE);
  }
  const char *hashedvalue = argv[1];
  const char *dictionaryfile = argv[2];
  //Validate hashedvalue & dictionarfile
  printf("dict: %s\n", dictionaryfile);
  printf("hash: %s\n", hashedvalue);
  //Exctract type and salt from hashedvalue
  char *p_type = malloc(HASH_ENCRYPTION_TYPE_SIZE + 1);
  char *p_salt = malloc(HASH_ENCRYPTION_SALT_SIZE + 1);
  strncpy(p_type, hashedvalue + HASH_ENCRYPTION_TYPE_START, HASH_ENCRYPTION_TYPE_SIZE);
  strncpy(p_salt, hashedvalue + HASH_ENCRYPTION_SALT_START, HASH_ENCRYPTION_SALT_SIZE);

  printf("salt: %s, type: %s\n", p_salt, p_type);
  int filesize = 0;//getfilesize(dictionaryfile);
  int words = 0;
  char **dictionary = NULL;// = malloc(sizeof(char**));
  load_dictionary(dictionaryfile,&dictionary,&words);

  printf("file-size=%d, words=%d,dictsize=%ld",filesize,words,sizeof(dictionary));
  free(p_type);
  free(p_salt);

  return 0;
}
int getfilesize(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    fseek(fp, 0, SEEK_END); // seek to end of file
    int size = ftell(fp);   // get current file pointer
    fclose(fp);
    return size;
}
void load_dictionary(const char *dictionaryfile, char*** dictionary, int* count)
{
  FILE *fp = fopen(dictionaryfile, "r");
  //size_t len = 0;
  char * buffer = malloc(sizeof(char)*30);
  
  if (fp == NULL)
    exit(EXIT_FAILURE);

  int position = 0;
  
  //fill buffer until we have a line
  char chard = fgetc(fp);
  int words = 0;
  int wordsize = 0;
  //*dictionary = malloc(sizeof(char*)); 
  while (chard != EOF)
  {
    //Add to buffer;
    if (chard == '\n') {
      //null terminate buffer
      buffer[wordsize-1] = '\0';
      //realoc size of dictionary
      *dictionary = realloc(*dictionary,sizeof(char*) * (words+1));
      (*dictionary)[words++] = malloc(wordsize+1);
      const char *result = strncpy((*dictionary)[words-1],buffer,wordsize);
      //if (words % 50 == 0)      
      //printf("%s\n",(*dictionary)[words-1]);//debug
      //reset wordsize
      wordsize = 0;
    }
    else {
      buffer[wordsize] = chard;
      wordsize ++;
    }
    position++;
    chard = fgetc(fp);
  }
  (*count) = words;
  //free up buffer
  free(buffer);
  //dictionary[(int)filesize] = '\0';
  fclose(fp);
}
/*
void bf_dictionary(char ***dictionary, char *passwd, char *salt)
{
}
void bf_normal()
{
}*/