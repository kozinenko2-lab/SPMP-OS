#ifndef SPMP_UI_POSIX_H
#define SPMP_UI_POSIX_H
#include "spmp_game_menu.h"
/* Test-host implementation only. Does NOT link into ARM standalone HAL. */
typedef struct {char root[256];} spmp_ui_posix;
int spmp_ui_posix_init(spmp_ui_posix *fs,const char *root);
spmp_ui_storage spmp_ui_posix_storage(spmp_ui_posix *fs);
#endif
