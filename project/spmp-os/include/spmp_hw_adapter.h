#ifndef SPMP_HW_ADAPTER_H
#define SPMP_HW_ADAPTER_H
#include <stddef.h>
#include <stdint.h>
#include "spmp_input_hal.h"
#include "defos_hal.h"
/* eCos libspmp8k mapping layout; documented in its libgame.h. */
typedef struct {int controller;uint32_t scancode[20];} spmp_legacy_keymap;
typedef int (*spmp_legacy_key_init)(spmp_legacy_keymap *);
typedef uint32_t (*spmp_legacy_key_get)(spmp_legacy_keymap *);
typedef int (*spmp_legacy_key_cleanup)(spmp_legacy_keymap *);

typedef struct {
 spmp_legacy_keymap legacy;
 spmp_hw_keymap mapping;
 spmp_legacy_key_get read;
 spmp_legacy_key_cleanup cleanup;
 unsigned ready;
} spmp_legacy_input;
/* No guessed addresses: requires imported eCos function pointers. */
int spmp_legacy_input_init(spmp_legacy_input *in,spmp_legacy_key_init init,
 spmp_legacy_key_get read,spmp_legacy_key_cleanup close);
uint32_t spmp_legacy_buttons(spmp_legacy_input *in);
uint32_t spmp_legacy_raw(spmp_legacy_input *in);
void spmp_legacy_input_close(spmp_legacy_input *in);
/* Only after the Power bit has been measured and verified on the target. */
void spmp_legacy_confirm_power(spmp_legacy_input *in,uint32_t mask,unsigned active_low);

typedef struct {
 void *user;
 uint32_t (*read_raw)(void *user);
 uint32_t (*clock_ms)(void *user);
 int (*present)(void *user,const uint16_t *rgb,unsigned w,unsigned h,unsigned pitch_bytes);
 int (*audio)(void *user,const int16_t *pcm,size_t n,unsigned hz);
 spmp_hw_keymap keys;
 uint32_t validated_ram_start,validated_ram_size;
 int hardware_verified;
} spmp_hw_adapter;
/* ARMv5-compatible HAL dispatch layer. No register access is assumed. */
int spmp_hw_adapter_ready(const spmp_hw_adapter *hal);
uint32_t spmp_hw_buttons(const spmp_hw_adapter *hal);
uint32_t spmp_hw_milliseconds(const spmp_hw_adapter *hal);
int spmp_hw_present(const spmp_hw_adapter *hal,const uint16_t *rgb,
 unsigned width,unsigned height,unsigned pitch);
#endif
