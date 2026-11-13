#ifndef SPMP_STRING_SHIM_H
#define SPMP_STRING_SHIM_H
#include <stddef.h>
void *memcpy(void *, const void *, size_t);
void *memchr(const void *, int, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);
int memcmp(const void *, const void *, size_t);
size_t strlen(const char *);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
char *strchr(const char *, int);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
#endif
