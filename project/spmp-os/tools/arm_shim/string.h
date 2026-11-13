#ifndef SPMP_ARM_STRING_H
#define SPMP_ARM_STRING_H
#include <stddef.h>
extern void *memset(void *, int, size_t);
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern int memcmp(const void *, const void *, size_t);
extern void *memchr(const void *, int, size_t);
extern size_t strlen(const char *);
extern int strcmp(const char *, const char *);
#endif
