#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
  if (argc < 2) {
    printf("Syntax: bruceforce <hashedvalue> <dictionary_file>\n");
    exit(EXIT_FAILURE);
  }
  const char * hashedvalue = argv[1];
  const char * dictionaryfile = argv[2];

  printf("dict: %s\n",dictionaryfile);
  printf("hash: %s\n",hashedvalue);
  
  return 0;
}
