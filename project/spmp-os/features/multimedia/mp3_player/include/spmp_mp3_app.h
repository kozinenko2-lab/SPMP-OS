/* App-facing Stage10 buttons + audio bridge; optional module, GPL-2.0-or-later. */
#ifndef SPMP_MP3_APP_H
#define SPMP_MP3_APP_H
#include "spmp_mp3_player.h"
#include "spmp_mp3_stage10.h"
#include "defos_hal.h"
typedef struct {
 union {uint64_t align;uint8_t bytes[SPMP_MP3_CONTEXT_MAX_BYTES];} arena;
 spmp_mp3_player *player;
 spmp_mp3_stage10_sink sink;
 unsigned volume;
 uint32_t prev_keys;
 int active;
} spmp_mp3_app;
int spmp_mp3_app_open(spmp_mp3_app *app,const spmp_mp3_io *io,const spmp_multimedia_driver *driver);
/* 1 playing, 2 paused, 0 finished/exit, <0 error. Call from normal UI loop. */
int spmp_mp3_app_tick(spmp_mp3_app *app,uint32_t buttons);
void spmp_mp3_app_close(spmp_mp3_app *app);
#endif
