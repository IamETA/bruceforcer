#ifndef FUNCTIONS_H_ /* Include guard */
#define FUNCTIONS_H_
void load_dictionary(const char *dictionaryfile, char ***dictionary,int *count);
//File operations
int getfilesize(const char *filename);
void getfiles(char ***files, int *count, const char *directory);
//String operations
char *concat(const char *s1, const char *s2);
#endif // FUNCTIONS_H_
