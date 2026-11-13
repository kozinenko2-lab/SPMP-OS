#ifndef FM01_POSIX_H
#define FM01_POSIX_H
#include "spmp_fm01.h"
typedef struct {int rootfd;} fm01_posix;
int fm01_posix_init(fm01_posix *fs,const char *root);
void fm01_posix_close(fm01_posix *fs);
int fm01_posix_list(void *context,const char *relative_path,uint32_t offset,
    uint32_t capacity,fm01_entry *out,uint32_t *returned,uint32_t *total,
    uint32_t *file_count,uint32_t *dir_count);
#endif
