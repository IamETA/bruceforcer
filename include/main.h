
#ifndef MAIN_H_ /* Include guard */
#define MAIN_H_
int getfilesize(const char *filename);
void load_dictionary(const char *dictionaryfile, char ***dictionary,int *count);
void bf_dictionary(char*** dictionary,char* passwd, char* salt);
#endif // MAIN_H_
