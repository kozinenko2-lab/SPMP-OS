#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void *malloc(size_t);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);
void free(void *);
void abort(void);
void exit(int);
int abs(int);
long labs(long);
int atoi(const char *);
int rand(void);
void srand(unsigned);
#define RAND_MAX 2147483647
#ifdef __cplusplus
}
#endif
