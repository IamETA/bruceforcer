#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include "include/functions.h"

extern char *strdup(const char *src);
//Dictionary
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
  int words = (*count);
  int wordsize = 0;
  //*dictionary = malloc(sizeof(char*)); 
  while (chard != EOF)
  {
    //Add to buffer;
    if (chard == '\n') {
      //null terminate buffer
      buffer[wordsize] = '\0';
      //realoc size of dictionary
      *dictionary = realloc(*dictionary,sizeof(char*) * (words+1));
      (*dictionary)[words++] = malloc(wordsize+1);
      strncpy((*dictionary)[words-1],buffer,wordsize+1);
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


//File operations
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

//Get files in directory
void getfiles(char ***files, int *count, const char *directory)
{
    DIR *dpdf;
    struct dirent *epdf;
    dpdf = opendir(directory);
    if (dpdf == NULL) {
      printf("Dictionaries folder not found: %s\n",directory);
        return;

    }
    //Folder has files, enumerate
    int fileCount = 0;
    while ((epdf = readdir(dpdf)))
    {
        //Dropping two bytes in 2 blocks somewhere in this code, don't know where.
        char *directoryName = malloc(strlen(epdf->d_name) + 1); //= epdf->d_name; //tried pointing to this, but that didnt work well
        strcpy(directoryName, epdf->d_name);
        if ((directoryName[0] != '.'))
        {
            *files = realloc(*files, sizeof(char *) * (fileCount + 1)); //again, +1 for nullterminator
            (*files)[fileCount++] = directoryName;
            printf("Found: %s\n", (*files)[fileCount - 1]);
        }
        else
        {
            //Free the unwanted directories
            free(directoryName);
        }
    }
    (*count) = fileCount;
    closedir(dpdf);
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