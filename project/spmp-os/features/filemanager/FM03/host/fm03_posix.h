#ifndef FM03_POSIX_H
#define FM03_POSIX_H
#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <sys/types.h>
#include "spmp_fm03.h"
typedef struct {
    int rootfd;
    dev_t device;
    DIR *frames[FM03_MAX_SCAN_DEPTH+1u];
    unsigned depth;
    fm03_scan stats;
} fm03_posix;
int fm03_posix_init(fm03_posix*,const char*root);
void fm03_posix_close(fm03_posix*);
void fm03_posix_backend(fm03_backend*);
#endif
