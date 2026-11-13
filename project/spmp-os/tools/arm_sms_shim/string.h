#ifndef SPMP_ARM_SMS_STRING_H
#define SPMP_ARM_SMS_STRING_H
#include <stddef.h>
void *memset(void*,int,size_t);
void *memcpy(void*,const void*,size_t);
void *memmove(void*,const void*,size_t);
int memcmp(const void*,const void*,size_t);
size_t strlen(const char*);
int strcmp(const char*,const char*);
char *strchr(const char*,int);
char *strrchr(const char*,int);
char *strstr(const char*,const char*);
#endif
