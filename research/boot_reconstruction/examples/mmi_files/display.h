#ifndef SPMP_FILES_DISPLAY_H
#define SPMP_FILES_DISPLAY_H
#include "input_test.h"
int spmp_files_rect(void *,unsigned,unsigned,unsigned,unsigned,uint32_t);
int spmp_files_text(void *,unsigned,unsigned,const char *,unsigned,uint32_t);
void spmp_files_font_open(const struct input_services *);
void spmp_files_font_close(void);
extern uint32_t spmp_files_font_mode;
#endif
