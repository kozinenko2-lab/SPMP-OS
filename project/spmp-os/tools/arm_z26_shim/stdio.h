/* Build-only declarations for relocatable ARM object. Not a runtime libc. */
#ifndef SPMP_Z26_STDIO_H
#define SPMP_Z26_STDIO_H
#include <stddef.h>
#define SEEK_SET 0
typedef struct spmp_file FILE;
extern FILE *stderr;
extern FILE *stdout;
int fprintf(FILE *,const char *,...);
int printf(const char *,...);
int sprintf(char *,const char *,...);
int puts(const char *);
FILE *fopen(const char *,const char *);
int fclose(FILE *);
int fseek(FILE *,long,int);
size_t fwrite(const void *,size_t,size_t,FILE *);
int getc(FILE *);
#endif
