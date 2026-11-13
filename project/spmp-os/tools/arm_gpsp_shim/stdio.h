#ifndef SPMP_GPSP_STDIO_H
#define SPMP_GPSP_STDIO_H
#include <stdarg.h>
typedef struct spmp_gpsp_FILE FILE;
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#include <stddef.h>
int sscanf(const char *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int sprintf(char *, const char *, ...);
int printf(const char *, ...);
#endif
