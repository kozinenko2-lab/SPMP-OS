#pragma once
#include <stddef.h>
typedef void *gzFile;
#ifdef __cplusplus
extern "C" {
#endif
int gzwrite(gzFile,const void*,unsigned);
int gzread(gzFile,void*,unsigned);
gzFile gzopen(const char *,const char *);
int gzclose(gzFile);
long gzseek(gzFile,long,int);
#ifdef __cplusplus
}
#endif
