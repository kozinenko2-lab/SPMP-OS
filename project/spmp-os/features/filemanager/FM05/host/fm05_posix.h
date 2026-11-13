/* SPDX-License-Identifier: MIT */
#ifndef SPMP_FM05_POSIX_H
#define SPMP_FM05_POSIX_H
#include "spmp_fm05.h"
typedef struct {int rootfd;unsigned temp_sequence;} fm05_posix;
int fm05_posix_init(fm05_posix *ctx,const char *root);
void fm05_posix_close(fm05_posix *ctx);
int fm05_posix_load(void *ctx,const char *path,char *out,unsigned cap,unsigned *n,uint64_t *revision);
int fm05_posix_save(void *ctx,const char *path,const char *text,unsigned size,
                     uint64_t expected,uint64_t *revision);
#endif
