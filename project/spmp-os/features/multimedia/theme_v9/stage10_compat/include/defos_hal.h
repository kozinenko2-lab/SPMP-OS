#ifndef DEFOS_HAL_H
#define DEFOS_HAL_H
#include <stdint.h>
#include <stddef.h>

/* Platform-facing interface. No eCos or Linux types in this API. */
enum { DEFOS_KEY_UP=1u<<0, DEFOS_KEY_DOWN=1u<<1,
       DEFOS_KEY_LEFT=1u<<2, DEFOS_KEY_RIGHT=1u<<3,
       DEFOS_KEY_A=1u<<4, DEFOS_KEY_B=1u<<5,
       DEFOS_KEY_START=1u<<6, DEFOS_KEY_SELECT=1u<<7 };

typedef struct {
  uint32_t (*read_buttons)(void *hardware);
  int (*video_present_rgb565)(void *hardware,const uint16_t *pixels,
                              unsigned width,unsigned height,unsigned pitch_bytes);
  int (*audio_submit_s16)(void *hardware,const int16_t *pcm,
                          size_t stereo_frames,unsigned sample_rate);
  int (*sd_ready)(void *hardware);
  void (*yield)(void *hardware);
  void *hardware;
} defos_hal;

/* ARMv5 standalone drawing test; no hardware IO. */
void defos_draw_safety_pattern(uint16_t *dst, unsigned width,
                               unsigned height, unsigned pitch_pixels,
                               unsigned frame_number);
#endif
