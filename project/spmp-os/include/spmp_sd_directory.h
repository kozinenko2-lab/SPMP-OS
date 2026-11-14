#ifndef SPMP_SD_DIRECTORY_H
#define SPMP_SD_DIRECTORY_H
#include "spmp_fm01.h"
#define SPMP_SD_SCAN_MAX 4096u
/* Absolute-path, read-only filesystem callbacks. Directory handles are positive. */
struct spmp_sd_meta {uint32_t mode,bytes;};
struct spmp_sd_io {
 void *user;
 int (*open)(void *,const char *);
 int (*next)(void *,int,char name[260]); /* 1 record, 0 EOF, negative error */
 int (*stat)(void *,const char *,struct spmp_sd_meta *);
 int (*close)(void *,int);
};
struct spmp_sd_directory {
 struct spmp_sd_io io;
 char root[64],path[FM01_PATH_MAX],absolute[FM01_PATH_MAX],name[260];
 fm01_entry page[FM01_PAGE_ROWS],floor,candidate;
 uint32_t offset,count,total,files,dirs,valid,scans;
};
int spmp_sd_directory_init(struct spmp_sd_directory *,const struct spmp_sd_io *,const char *root);
void spmp_sd_directory_invalidate(struct spmp_sd_directory *);
int spmp_sd_path(const struct spmp_sd_directory *,const char *relative,char *out,size_t capacity);
int spmp_sd_list_page(void *,const char *,uint32_t,uint32_t,fm01_entry *,uint32_t *,uint32_t *,uint32_t *,uint32_t *);
#endif
