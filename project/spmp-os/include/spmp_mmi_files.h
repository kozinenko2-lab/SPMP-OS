#ifndef SPMP_MMI_FILES_H
#define SPMP_MMI_FILES_H
#include "spmp_mmi_input.h"
#include "spmp_sd_directory.h"
struct spmp_mmi_files {
 fm01_model browser;
 struct spmp_sd_directory directory;
 char selected_path[FM01_PATH_MAX];
 uint32_t selected_atari,details,changes;
};
uint32_t spmp_mmi_fm_buttons(uint32_t);
int spmp_mmi_files_init(struct spmp_mmi_files *,const struct spmp_sd_io *);
/* 1 requests home at SD root/START; 0 stays in browser. POWER handled by shell. */
int spmp_mmi_files_update(struct spmp_mmi_files *,const struct spmp_mmi_frame *);
struct spmp_file_draw {
 void *user;
 int (*rect)(void *,unsigned,unsigned,unsigned,unsigned,uint32_t rgb);
 int (*text)(void *,unsigned,unsigned,const char *,unsigned,uint32_t rgb);
};
int spmp_mmi_files_draw(const struct spmp_mmi_files *,const struct spmp_file_draw *);
#endif
