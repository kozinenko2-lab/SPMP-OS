#ifndef SPMP_THEME_CONFIG_V8_H
#define SPMP_THEME_CONFIG_V8_H
#include <stdint.h>
#define SPMP_THEME_V8_CONFIG_BYTES 40u
/* Persistent selection: stable text ID, layout, checked CRC32. Host chooses where to save.
 * All functions are freestanding; no filesystem, memory allocations or Linux calls.
 */
int spmp_theme_v8_config_encode(const char *theme_id,uint8_t layout,uint8_t out[SPMP_THEME_V8_CONFIG_BYTES]);
int spmp_theme_v8_config_decode(const uint8_t in[SPMP_THEME_V8_CONFIG_BYTES],unsigned *index,uint8_t *layout);
unsigned spmp_theme_v8_cycle(unsigned selected,int delta);
#endif
