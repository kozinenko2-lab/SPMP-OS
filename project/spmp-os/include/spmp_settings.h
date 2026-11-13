/* SPMP-OS shared, versioned per-emulator settings. No POSIX dependency. */
#ifndef SPMP_SETTINGS_H
#define SPMP_SETTINGS_H
#include <stddef.h>
#include <stdint.h>
#define SPMP_SETTINGS_MAX 9u
#define SPMP_SETTINGS_DISK_BYTES 128u

typedef enum {
 SPMP_OPT_SCALE=1, SPMP_OPT_VOLUME, SPMP_OPT_SWAP_AB,
 SPMP_OPT_FRAMESKIP, SPMP_OPT_REGION, SPMP_OPT_GB_PALETTE,
 SPMP_OPT_SPRITE_LIMIT, SPMP_OPT_GG_BORDER, SPMP_OPT_CPU_ACCURACY,
 SPMP_OPT_TIA_PALETTE, SPMP_OPT_MACHINE_MODEL, SPMP_OPT_USE_BIOS
} spmp_setting_id;

typedef struct {
 spmp_setting_id id;
 const char *label;
 int min_value,max_value,default_value;
 /* Immediately effective only when a system/core callback handles it. */
 unsigned deferred;
} spmp_setting_def;

typedef struct {
 void *ctx;
 int (*load)(void *ctx,const char *path,uint8_t *data,size_t size,size_t *actual);
 int (*save_atomic)(void *ctx,const char *path,const uint8_t *data,size_t size);
 /* Returns 0 if applied, -1 if invalid/error, +1 if saved for next boot. */
 int (*apply)(void *ctx,spmp_setting_id id,int value);
} spmp_settings_io;

typedef struct {
 spmp_setting_def defs[SPMP_SETTINGS_MAX];
 int values[SPMP_SETTINGS_MAX];
 unsigned count;
 char path[64];
 uint32_t core_hash;
 spmp_settings_io io;
 int dirty; /* deferred application or persistence unavailable */
} spmp_settings;

int spmp_settings_init(spmp_settings *s,const char *core_id,const spmp_settings_io *io);
int spmp_settings_read(spmp_settings *s);
int spmp_settings_write(spmp_settings *s);
/* Delta is +1/-1, wraps in range; persistence failure rolls back value. */
int spmp_settings_change(spmp_settings *s,unsigned index,int delta);
int spmp_settings_get(const spmp_settings *s,spmp_setting_id id,int fallback);
const char *spmp_settings_value(const spmp_settings *s,unsigned index,char *buf,size_t capacity);
#endif
