/* SPMP-OS Multimedia Extension v1: no coupling to emulator cores. */
#ifndef SPMP_MM_API_H
#define SPMP_MM_API_H
#include <stddef.h>
#include <stdint.h>
#define SPMP_MM_API 1u
#define SPMP_MM_W 320u
#define SPMP_MM_H 240u
#define SPMP_MM_MAX_PHOTO_SIDE 4096u
#define SPMP_MM_MAX_PHOTO_PIXELS 4194304u

typedef enum {
 MM_UNKNOWN=0, MM_WAV, MM_MP3, MM_MP4, MM_AVI, MM_BMP, MM_PNG,
 MM_JPEG, MM_GIF
} mm_type;
typedef struct {
 void *ctx;
 uint64_t size;
 /* Exactly count bytes or return <0. 0=success. Must not read beyond size. */
 int (*read_at)(void *ctx, uint64_t offset, void *dst, size_t count);
} mm_reader;
typedef struct {
 mm_type type;
 uint32_t width,height;
 uint32_t sample_rate, channels, bits_per_sample;
 uint32_t duration_ms; /* 0 means unknown, never guessed from CBR */
 uint32_t data_bytes;
 char audio_codec[12];
 char video_codec[12];
 char label[16];
} mm_info;

typedef struct {
 uint16_t bg, panel, panel_alt, accent, accent_alt, text, muted, border;
 uint16_t active, highlight;
 char name[32];
} mm_skin;

typedef struct {
 void *ctx;
 int (*submit_s16)(void *ctx,const int16_t *interleaved,size_t stereo_frames,unsigned sample_rate);
 /* 0 = continue. Negative = cancel/error. May be called repeatedly. */
} mm_audio_output;

int mm_inspect(const mm_reader *reader,mm_info *out);
/* Streaming WAV mono/stereo PCM8/16 -> stereo signed 16; no heap, bounded stack. */
int mm_play_wav(const mm_reader *reader,const mm_audio_output *output, uint32_t *frames_done);
/* BMP BI_RGB 24/32 -> RGB565, letterboxed to screen, row buffer max 4096 bytes. */
int mm_render_bmp(const mm_reader *reader,uint16_t *fb,unsigned pitch_pixels,
                  unsigned screen_w,unsigned screen_h);
void mm_skin_default(mm_skin *out);
/* Text INI subset. No file IO, no paths, no heap, transactional parser. */
int mm_skin_parse(mm_skin *skin,const char *text,size_t bytes);
/* Portable RGB565 home UI. Optional status string and selection index [0..5]. */
void mm_draw_home(uint16_t *fb,unsigned pitch_pixels,const mm_skin *skin,
                  unsigned selected,const char *status);
const char *mm_type_name(mm_type type);
#endif
