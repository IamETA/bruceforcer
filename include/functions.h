#ifndef FUNCTIONS_H_ /* Include guard */
#define FUNCTIONS_H_
//Declerations
#define ALPHABET "abcdefghikjlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890+\\\"#&/()=?!@$|][{}<>.,-"
//Bruceforce operations
typedef struct dict_args
{
  int id;
  int segment_count;
  char ***dictionary;
  const char *salt;
  const char *hash;
  int segment_from;
  float p_status;
  const char * password;
  bool stop;
} dict_args;

typedef struct bruteforce_args
{
  int id;
  const char *c_table;
  int c_tablesize;
  int wordsize;
  const char *salt;
  const char *hash;
  int segment_from;
  int segment_count;
  float p_status;
  long p_processed;
  char *password;
  bool stop;
  int activesegment;
} bruteforce_args;

const char* bf_dictionary(char ***dictionary,int segment_from, int segment_count, const char *p_type_salt, const char *hashedvalue, float *p_status, bool *abort);
char *bf_hack(bruteforce_args *args);
void load_dictionary(const char *dictionaryfile, char ***dictionary,int *segment_count,int *dictfilecount);
//File operations
int getfilesize(const char *filename);
void getfiles(char ***files, int *segment_count, const char *directory);
//String operations
char *concat(const char *s1, const char *s2);
#endif // FUNCTIONS_H_
