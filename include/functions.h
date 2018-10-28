#ifndef FUNCTIONS_H_ /* Include guard */
#define FUNCTIONS_H_
//Bruceforce operations
typedef struct dict_args
{
  int id;
  int count;
  char ***dictionary;
  char *salt;
  const char *hash;
  int startfrom;
  float p_status;
  char * password;
  bool stop;
} dict_args;

char* bf_dictionary(char ***dictionary,int startfrom, int count, char *p_type_salt, const char *hashedvalue, float *p_status, bool *abort);

void load_dictionary(const char *dictionaryfile, char ***dictionary,int *count);
//File operations
int getfilesize(const char *filename);
void getfiles(char ***files, int *count, const char *directory);
//String operations
char *concat(const char *s1, const char *s2);
#endif // FUNCTIONS_H_
