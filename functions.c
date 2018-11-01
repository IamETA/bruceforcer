//
#define _GNU_SOURCE

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <crypt.h>
#include "include/functions.h"

extern char *strdup(const char *src);

//Bruceforce operations
void load_dictionary_item(const char *dictionaryfile, char ***dictionary, int *count);
char *password_for_hash(struct crypt_data *cdata,const char *salt, const char *hash, char* password);
const char *bf_dictionary(char ***dictionary, int startfrom, int count, const char *p_type_salt, const char *hashedvalue, float *p_status, bool *abort)
{
  // Crypto Init
  struct crypt_data *cdata = malloc(sizeof(struct crypt_data));
  //struct crypt_data cdata;
  cdata->initialized = 0;
  for (int i = startfrom; i < startfrom + count; i++)
  {
    if (*abort == true)
      break;
    
    if (password_for_hash(cdata,p_type_salt,hashedvalue,(*dictionary)[i]) != NULL) {
      return (*dictionary)[i];
    }
    *p_status = ((float)(i - startfrom) / count) * 100;
  }
  return '\0';
}

/* 
  Check password to hash function to see if the
  hashed result is equal to the input of <hash>

  returns NULL if the result does not match the hash
    or POINTER to the maching password if it
    successfully matches the input string.

  Improvements: We can probably optimize the string compare 
  function to increase the speed
*/
char *password_for_hash(struct crypt_data *cdata,const char *salt, const char *hash,char* password) {
  //printf("%s ", password);
  char* hashcompare = crypt_r(password,salt,cdata);
  if (strcmp(hashcompare,hash) == 0) {
    return password;
  }
  else {
    return NULL;
  }
}
/* This is the function that will enumerate all
  possible characters in a given c_tablesize.
  Like smoking out hiding places, no characters can hide
  from this function :)

  This was the hardest function to make, and theres plenty of 
  optimizations to make
*/
char* bytesmoker(bruteforce_args *bargs,char* sz_word, int wordsize,int workingposition,struct crypt_data *cdata) {
  for (int i = 0;i<bargs->c_tablesize;i++) {
    if (bargs->stop) return NULL;
    // feedback , for best optimization, remove this (if run with no output fex)
    if (workingposition == 1) bargs->p_status = ((float)((float)i/(float)bargs->c_tablesize))*100;

    //Go deeper if possible
    if (workingposition < wordsize) {
      //printf("wordsize:%i",wordsize);
      char* password = bytesmoker(bargs,sz_word,wordsize,workingposition+1,cdata);
      if (password != NULL) return password;
    }

    //Change the character
    sz_word[workingposition] = bargs->c_table[i];

    //Test the password
    bargs->p_processed++; //Show number of processed words
    if (password_for_hash(cdata,bargs->salt,bargs->hash,sz_word) != NULL) 
      return sz_word;
  }

  return NULL;
}


char *bf_hack(bruteforce_args *args)
{
  // Crypto Init
  char *return_password;
  struct crypt_data *cdata = malloc(sizeof(struct crypt_data));
  cdata->initialized = 0;
  
  char *sz_word = malloc(sizeof(char*)); //Allocate buffer
  for (int segment = args->segment_from;segment<args->segment_from + args->segment_count;segment++) {
    //Start with the first character in c_table
    //"statically" assign first bit with segment (each thread will have different segments)
    sz_word[0] = args->c_table[segment]; //As multithread, we only process a segment of the total c_tablesize
    for (int wordsize=1;wordsize<args->wordsize;wordsize++) {
      if (args->stop) {
        //REMEMBER TO FREE MEMORY here
        return NULL;
      }
      sz_word = realloc(sz_word,sizeof(char*) * wordsize);
      //sz_word[wordsize]=args->c_table[0];
      sz_word[wordsize+1]='\0';
      //Process each column
      int charpos = 1;
      return_password = bytesmoker(args, sz_word, wordsize, charpos, cdata);
      
      //Remember to free up memory if needed here
      if(return_password!=NULL) return return_password;
    }
    args->activesegment = segment - args->segment_from;
    args->p_status = ((float)(segment - args->segment_from) / args->segment_count) * 100;
  }
  return NULL;
}
//Dictionary
void load_dictionary(const char *dictionarypath, char ***dictionary, int *count, int *dictfilecount)
{
  (*dictfilecount) = 0;
  DIR *dpdf;
  struct dirent *epdf;
  dpdf = opendir(dictionarypath);
  if (dpdf == NULL)
  {
    printf("Dictionaries folder not found: %s\n", dictionarypath);
    return;
  }
  while ((epdf = readdir(dpdf)))
  {
    if (epdf->d_name[0] != '.') {
      char *dictionaryfilepath = concat(dictionarypath, epdf->d_name);
      load_dictionary_item(dictionaryfilepath, dictionary, count);
      (*dictfilecount)++;
      free(dictionaryfilepath);
    }
  }
  closedir(dpdf);
}

void load_dictionary_item(const char *dictionaryfile, char ***dictionary, int *count)
{

  FILE *fp = fopen(dictionaryfile, "r");
  //size_t len = 0;
  char *buffer = malloc(sizeof(char) * 30);

  if (fp == NULL)
    exit(EXIT_FAILURE);

  int position = 0;

  //fill buffer until we have a line
  char chard = fgetc(fp);
  int words = (*count);
  int wordsize = 0;
  //*dictionary = malloc(sizeof(char*));
  while (chard != EOF)
  {
    //Add to buffer;
    if (chard == '\n')
    {
      //null terminate buffer
      buffer[wordsize] = '\0';
      //realoc size of dictionary
      *dictionary = realloc(*dictionary, sizeof(char *) * (words + 1));
      (*dictionary)[words++] = malloc(wordsize + 1);
      strncpy((*dictionary)[words - 1], buffer, wordsize + 1);
      wordsize = 0;
    }
    else
    {
      buffer[wordsize] = chard;
      wordsize++;
    }
    position++;
    chard = fgetc(fp);
  }
  (*count) = words;
  free(buffer);
  fclose(fp);
}

//String operations
//Combine strings, in a somewhat sloppy way, but hey
char *concat(const char *s1, const char *s2)
{
  size_t stringsize = strlen(s1) + strlen(s2) + 1;
  char *result = malloc(stringsize); // +1 for the null-terminator
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}