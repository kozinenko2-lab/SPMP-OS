#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void *memset(void *,int,size_t);
void *memcpy(void *,const void *,size_t);
void *memmove(void *,const void *,size_t);
int memcmp(const void *,const void *,size_t);
size_t strlen(const char *);
char *strcpy(char *,const char *);
char *strncpy(char *,const char *,size_t);
int strcmp(const char *,const char *);
int strncmp(const char *,const char *,size_t);
char *strchr(const char *,int);
char *strrchr(const char *,int);
char *strcat(char *,const char *);
char *strncat(char *,const char *,size_t);
char *strpbrk(const char*,const char*);
size_t strspn(const char*,const char*);
size_t strcspn(const char*,const char*);
char *strtok(char*,const char*);
char *strstr(const char*,const char*);
void *memchr(const void*,int,size_t);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
int strncasecmp(const char *,const char *,size_t);
#ifdef __cplusplus
}
#endif
