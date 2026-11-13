/* Optional Stage10 bridge. GPL-2.0-or-later, independent of emulator sources. */
#ifndef SPMP_MP3_STAGE10_H
#define SPMP_MP3_STAGE10_H
#include "spmp_mp3_player.h"
#include "spmp_multimedia_hal.h"
typedef struct {
 const spmp_multimedia_driver *hal;
 unsigned rate,channels;
 int opened;
} spmp_mp3_stage10_sink;
void spmp_mp3_stage10_init(spmp_mp3_stage10_sink *sink,const spmp_multimedia_driver *hal);
int spmp_mp3_stage10_pcm(void *sink,const int16_t *pcm,size_t frames,unsigned rate,unsigned channels);
void spmp_mp3_stage10_close(spmp_mp3_stage10_sink *sink);
#endif
