#ifndef SPMP_PICO_STDIO_H
#define SPMP_PICO_STDIO_H
#include <stddef.h>
typedef struct SPMP_FILE FILE;
#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
extern FILE *fopen(const char*,const char*);
extern int fclose(FILE*);
extern int fseek(FILE*,long,int);
extern long ftell(FILE*);
extern size_t fread(void*,size_t,size_t,FILE*);
extern size_t fwrite(const void*,size_t,size_t,FILE*);
extern char *fgets(char*,int,FILE*);
extern int printf(const char*,...);
extern int fprintf(FILE*,const char*,...);
extern int sprintf(char*,const char*,...);
extern int snprintf(char*,size_t,const char*,...);
extern int puts(const char*);
extern int remove(const char*);
extern FILE *stderr;
extern int feof(FILE*);
#endif
