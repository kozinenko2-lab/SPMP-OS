#ifndef SPMP_ARM_SMS_STDIO_H
#define SPMP_ARM_SMS_STDIO_H
#include <stddef.h>
typedef struct spmp_file FILE;
int printf(const char*, ...);
int sprintf(char *,const char *,...);
int snprintf(char*,size_t,const char*,...);
FILE *fopen(const char*,const char*);
int fclose(FILE*);
size_t fread(void *,size_t,size_t,FILE*);
size_t fwrite(const void*,size_t,size_t,FILE*);
int fseek(FILE*,long,int);
#endif
