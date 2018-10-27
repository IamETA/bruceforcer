#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/main.h"
#include <sys/time.h>
#include "include/crypto.h"
#include "include/functions.h"
#include <crypt.h>

#define HASH_SALT_SIZE 11

int main(int argc, char const *argv[])
{
  if (argc < 2)
  {
    printf("Syntax: bruceforce <hashedvalue> <optional dictionarypath>\n");
    exit(EXIT_FAILURE);
  }
  const char *hashedvalue = argv[1];
  //Exctract type and salt from hashedvalue
  char *p_type_salt = malloc(HASH_SALT_SIZE);
  strncpy(p_type_salt,argv[1], HASH_SALT_SIZE);

  
  printf("hash: %s\n", hashedvalue);
  printf("salt: %s\n", p_type_salt);
  
  const char *dictpath = "./dictionaries/";
  if (argc > 2) {
    dictpath = argv[2];
  }

  //Load Dictionaries
  int dictfilecount = 0;
  char **dictionaries = NULL;
  printf("Reading dictionary folder: %s\n",dictpath);
  getfiles(&dictionaries,&dictfilecount,dictpath);
  
  int words = 0;
  char **dictionary = NULL;// = malloc(sizeof(char**));

  for(int i=0;i<dictfilecount;i++){
    printf("Loading dictionary %s\n",dictionaries[i]);
    char *dictionaryfilepath = concat(dictpath,dictionaries[i]);
    load_dictionary(dictionaryfilepath,&dictionary,&words);
    free(dictionaryfilepath);
  }
  printf("dictionary-word-count=%d, total-files=%d\n",words,dictfilecount);
  //Validate hashedvalue & dictionarfile
  char * bf_dict_result = bf_dictionary(&dictionary,words,p_type_salt,hashedvalue);
  if (strlen(bf_dict_result) > 0) {
    printf("Password was found: %s\n",bf_dict_result);
  }
  else {
    printf("No password was found in dictionary, continuing to bruteforcing...\n");
  }
  
  printf("Complete.");
  free(p_type_salt);

  return 0;
}

char* bf_dictionary(char ***dictionary, int count, char *p_type_salt, const char *hashedvalue)
{
  float percentage;
  float lastprintpercent = 0;

  for (int i=0;i<count-1;i++) {
    const char *testpwd = crypt((*dictionary)[i],p_type_salt);
    if ((percentage = ((float)i / count) * 100) > (lastprintpercent + 1)) {
      lastprintpercent = percentage;
      printf("%.2f Percent Complete: %d / %d \n",lastprintpercent,i,count);
    }

    #ifdef DEBUG
    printf("testing word:%s -> %s against %s\n",(*dictionary)[i],testpwd,hashedvalue);
    #endif

    if (strcmp(testpwd,hashedvalue) == 0) {
      printf("Password could be: %s\n",(*dictionary)[i]);
      return((*dictionary)[i]);
    }
  }  
  return '\0';
}
/*
void bf_normal()
{
}*/