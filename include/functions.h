#ifndef FUNCTIONS_H_ /* Include guard */
#define FUNCTIONS_H_
typedef struct POSITION
{
    int x;
    int y;
} POSITION;
//Merger operations
void mapfilepositions(POSITION pos[],int filecount, char ** files);
int merge(char **outputfiledata,const char* outputfile, int FRAME_WIDTH,int mx,int my);
//File operations
int getfilesize(const char *filename);
void readfromfile(const char *fileName, char *fcontent);
void getfiles(char ***files, int *count, const char *directory);
void load_in_order(int mx, int my, char **files, int filecount, const char *folder, char *outputfiledata[], int filesize, POSITION pos[]);
//Math operations
int max_x(const POSITION a[], int n);
int max_y(const POSITION a[], int n);

//String operations
char *concat(const char *s1, const char *s2);

#endif // FUNCTIONS_H_
