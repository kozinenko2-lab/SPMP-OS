#ifndef SPMP_ARM_STDLIB_H
#define SPMP_ARM_STDLIB_H
#include <stddef.h>
extern void *malloc(size_t);
extern void free(void *);
#endif
extern void *calloc(size_t, size_t);
extern void *realloc(void *, size_t);
extern long strtol(const char *, char **, int);
extern int atoi(const char *);
