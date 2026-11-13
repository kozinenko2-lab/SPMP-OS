#ifndef SPMP_M3U_PLAYLIST_H
#define SPMP_M3U_PLAYLIST_H
#include "spmp_mp3_player.h"
#define SPMP_M3U_MAX_PATH 256
/* M3U/M3U8 iterator: bounded line reads from any read callback. */
typedef struct {
 spmp_mp3_io io;
 uint8_t block[512];
 size_t at,end;
 unsigned line_no;
 int eof;
} spmp_m3u_reader;
void spmp_m3u_open(spmp_m3u_reader *, const spmp_mp3_io *);
/* 1=path, 0=end, -1=malformed/oversized/read error */
int spmp_m3u_next(spmp_m3u_reader *,char out[SPMP_M3U_MAX_PATH]);
#endif
