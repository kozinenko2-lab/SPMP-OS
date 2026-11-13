#ifndef SPMP_FM02_POSIX_H
#define SPMP_FM02_POSIX_H
#include "spmp_fm02.h"
#include <sys/types.h>
typedef struct {
    int rootfd;
    dev_t device;
    unsigned seq;
} fm02_posix;
int fm02_posix_init(fm02_posix *fs,const char *root);
void fm02_posix_close(fm02_posix *fs);
int fm02_posix_action(void *ctx,const fm02_request *req,
    fm02_progress_hook hook,void *hook_ctx,fm02_progress *result);
#endif
