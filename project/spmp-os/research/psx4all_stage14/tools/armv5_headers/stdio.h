#pragma once
#include <stddef.h>
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct _spmp_FILE FILE;
extern FILE *stderr;
int printf(const char *, ...);
int fprintf(FILE *, const char *, ...);
int vfprintf(FILE *, const char *,va_list);
int sprintf(char *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int sscanf(const char *,const char *,...);
int fseek(FILE *, long, int);
long ftell(FILE *);
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define EOF (-1)
int puts(const char *);
int putchar(int);
int getchar(void);
FILE *fopen(const char *, const char *);
int fclose(FILE *);
size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int fgetc(FILE *);
int ferror(FILE *);
#ifdef __cplusplus
}
#endif
