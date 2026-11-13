#ifndef SPMP_STDIO_SHIM_H
#define SPMP_STDIO_SHIM_H
#include <stddef.h>
typedef struct spmp_FILE FILE;
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
FILE *fopen(const char *,const char *);
int fclose(FILE *);
size_t fread(void *, size_t, size_t, FILE *);
int fseek(FILE *, long, int);
long ftell(FILE *);
#endif
