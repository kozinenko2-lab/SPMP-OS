/* SPMP-OS framebuffer conversion, ARMv5 portable C (no heap). */
#ifndef SPMP_SMALL_VIDEO_H
#define SPMP_SMALL_VIDEO_H
#include <stdint.h>
#include <stddef.h>
#define SPMP_VIDEO_RGB565 0u
#define SPMP_VIDEO_0RGB1555 1u
#define SPMP_VIDEO_XRGB8888 2u
/* Fit to target LCD preserving aspect ratio. Returns 0 on success. */
int spmp_video_fit(unsigned in_w,unsigned in_h,unsigned max_w,unsigned max_h,
                   unsigned *out_w,unsigned *out_h);
/* Convert and scale nearest-neighbor. source pitch is bytes, dest has out_w rows. */
int spmp_video_convert(const void *source,size_t pitch,unsigned in_w,unsigned in_h,
                       unsigned format,uint16_t *dest,unsigned out_w,unsigned out_h);
#endif
