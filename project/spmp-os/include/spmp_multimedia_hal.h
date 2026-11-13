#ifndef SPMP_MULTIMEDIA_HAL_H
#define SPMP_MULTIMEDIA_HAL_H
#include <stddef.h>
#include <stdint.h>
/* Contracts only: all device functions remain UNIMPLEMENTED in Stage 2.
   No codec, FM antenna or video hardware API is claimed functional. */
typedef struct spmp_multimedia_driver {
 void *context;
 int (*headphones_connected)(void *ctx); /* required before FM receive */
 int (*fm_tune_khz)(void *ctx,unsigned khz);
 int (*fm_scan)(void *ctx,int up,unsigned *khz);
 int (*audio_open)(void *ctx,unsigned sample_rate,unsigned channels);
 int (*audio_write_s16)(void *ctx,const int16_t *samples,size_t frames);
 int (*audio_close)(void *ctx);
 int (*video_open)(void *ctx,const char *path); /* container + codec driver */
 int (*video_next_rgb565)(void *ctx,uint16_t *dst,unsigned stride);
 void (*video_close)(void *ctx);
 int (*image_open)(void *ctx,const char *path);
 int (*image_render_rgb565)(void *ctx,uint16_t *dst,unsigned stride);
 void (*image_close)(void *ctx);
} spmp_multimedia_driver;
#endif
