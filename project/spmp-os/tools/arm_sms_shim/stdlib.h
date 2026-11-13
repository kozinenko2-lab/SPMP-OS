#ifndef SPMP_ARM_SMS_STDLIB_H
#define SPMP_ARM_SMS_STDLIB_H
#include <stddef.h>
void *malloc(size_t);
void *calloc(size_t,size_t);
void *realloc(void*,size_t);
void free(void*);
void abort(void);
int abs(int);
long labs(long);
#endif
