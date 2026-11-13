#ifndef SPMP_STDLIB_SHIM_H
#define SPMP_STDLIB_SHIM_H
#include <stddef.h>
void *malloc(size_t);
void *calloc(size_t,size_t);
void *realloc(void *,size_t);
void free(void *);
void abort(void);
int abs(int);
void qsort(void *, size_t, size_t, int (*)(const void*, const void*));
#endif
