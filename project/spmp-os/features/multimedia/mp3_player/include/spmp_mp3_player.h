/* SPMP-OS standalone MP3 plugin, ABI 1. GPL-2.0-or-later (libmad backend). */
#ifndef SPMP_MP3_PLAYER_H
#define SPMP_MP3_PLAYER_H
#include <stddef.h>
#include <stdint.h>

#define SPMP_MP3_ABI 1u
#define SPMP_MP3_IOBUF 8192u
#define SPMP_MP3_MAX_PCM_FRAMES 1152u
#define SPMP_MP3_CONTEXT_MAX_BYTES 49152u

/* Callback I/O: read returns 0 on EOF, <0 on error. seek takes absolute byte offset. */
typedef struct {
 void *ctx;
 int (*read)(void *, uint8_t *, size_t);
 int (*seek)(void *, uint64_t);  /* optional for ordinary playback */
 uint64_t (*size)(void *);      /* optional for estimated duration and seek */
} spmp_mp3_io;

typedef struct {
 char title[128], artist[128], album[128], genre[64], year[16], track[16];
 uint64_t audio_offset;
 uint32_t id3v2_bytes;
 uint32_t sample_rate, channels, bitrate_kbps;
 uint32_t duration_ms;          /* 0 if unavailable; CBR approximate, Xing preferred */
 uint8_t vbr;
 uint64_t cover_offset;    /* optional embedded APIC/PIC JPEG/PNG bytes */
 uint32_t cover_bytes;
 char cover_mime[24];
} spmp_mp3_metadata;

typedef int (*spmp_mp3_pcm_callback)(void *, const int16_t *interleaved,
                                     size_t frames, unsigned sample_rate,
                                     unsigned channels);

typedef struct spmp_mp3_player spmp_mp3_player;
/* Caller owns memory; no allocations by SPMP plugin or libmad when SPMP_MAD_STATIC=1. */
size_t spmp_mp3_player_size(void);
int spmp_mp3_open(spmp_mp3_player *, const spmp_mp3_io *,
                  spmp_mp3_pcm_callback, void *pcm_ctx);
/* 1 = decoded frame, 2 = paused, 0 = EOF, -1 = invalid/read error, -2 = sink failure. */
int spmp_mp3_step(spmp_mp3_player *);
/* Playback: call step once per UI/event loop iteration; never busy-loop while paused. */
void spmp_mp3_pause(spmp_mp3_player *, int paused);
int spmp_mp3_paused(const spmp_mp3_player *);
int spmp_mp3_seek_ms(spmp_mp3_player *, uint32_t ms); /* approximate frame-based seek */
void spmp_mp3_set_volume(spmp_mp3_player *, unsigned volume_0_to_256);
uint32_t spmp_mp3_position_ms(const spmp_mp3_player *);
const spmp_mp3_metadata *spmp_mp3_info(const spmp_mp3_player *);
uint32_t spmp_mp3_bad_frames(const spmp_mp3_player *);
void spmp_mp3_close(spmp_mp3_player *);

/* Utility: read ID3v2 and trailing ID3v1 when random access is available. */
int spmp_mp3_read_metadata(const spmp_mp3_io *, spmp_mp3_metadata *out);

#endif
