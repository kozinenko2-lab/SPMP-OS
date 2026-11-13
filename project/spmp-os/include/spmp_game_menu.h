#ifndef SPMP_GAME_MENU_H
#define SPMP_GAME_MENU_H
#include <stddef.h>
#include <stdint.h>
#include "spmp_theme.h"
#include "spmp_settings.h"

#define SPMP_UI_THUMB_W 120u
#define SPMP_UI_THUMB_H 90u
#define SPMP_UI_THUMB_PIXELS (SPMP_UI_THUMB_W * SPMP_UI_THUMB_H)
#define SPMP_UI_SLOTS 5u
#define SPMP_UI_STATE_MAX (8u * 1024u * 1024u)
#define SPMP_UI_PATH_MAX 160u
#define SPMP_UI_STATE_HEADER 48u

/* NORMALIZED keys, never assume raw hardware bits match these. */
enum { SPMP_UI_UP=1u, SPMP_UI_DOWN=2u, SPMP_UI_LEFT=4u,
       SPMP_UI_RIGHT=8u,SPMP_UI_CONFIRM=16u, SPMP_UI_BACK=32u,
       SPMP_UI_POWER=256u };

typedef enum {SPMP_UI_CONTINUE=0,SPMP_UI_SAVE=1,SPMP_UI_LOAD=2,
              SPMP_UI_SLOT=3,SPMP_UI_SETTINGS=4,SPMP_UI_EXIT=5,SPMP_UI_ITEMS=6} spmp_ui_action;

/* The platform MUST implement an atomic replace (temp + fsync + rename).
 * Only one .sps file per slot; state and game-frame thumbnail are committed
 * together, so menu preview cannot get out of sync with savestate. */
typedef struct {
 void *user;
 int (*store_atomic)(void *user,const char *path,const uint8_t *src,size_t n);
 int (*load)(void *user,const char *path,uint8_t *dst,size_t capacity,size_t *actual);
 /* Optional fast read: only header + thumbnail, no multi-MiB savestate. */
 int (*load_prefix)(void *user,const char *path,uint8_t *dst,size_t exact_bytes);
} spmp_ui_storage;

typedef struct {
 void *user;
 size_t (*state_size)(void *user);   /* 0 means unsupported */
 int (*serialize)(void *user,void *dst,size_t capacity);
 int (*unserialize)(void *user,const void *src,size_t length);
 void (*pause)(void *user);
 void (*resume)(void *user);
 /* Optional: 0 applied, 1 deferred, -1 invalid. */
 int (*apply_setting)(void *user,spmp_setting_id option,int value);
 int (*exit_to_library)(void *user); /* 0 success */
} spmp_ui_emulator;

typedef struct {
 uint8_t *scratch;  /* caller-owned, >= 48+21600+core state bytes */
 size_t scratch_size;
 spmp_ui_storage storage;
 spmp_ui_emulator emulator;
 char core_id[32];
 char rom_id[96];
 uint16_t clean_frame[SPMP_UI_THUMB_PIXELS];
 uint16_t preview[SPMP_UI_THUMB_PIXELS];
 uint64_t core_hash, rom_hash;
 uint32_t power_down_at;
 uint32_t previous_buttons;
 unsigned slot;
 unsigned selection;
 unsigned settings_open;
 unsigned settings_selection;
 spmp_settings settings;
 unsigned power_was_down;
 unsigned held_long;
 unsigned clean_valid;
 unsigned preview_valid;
 unsigned opened;
 unsigned quit;
 int last_result;
} spmp_game_menu;

/* Power event is generated on RELEASE of 30..650ms, not at key-down.
 * Long presses are reserved for hardware power management. */
void spmp_menu_init(spmp_game_menu *m,const char *core_id,const char *rom_id,
 const spmp_ui_storage *storage,const spmp_ui_emulator *emu,
 uint8_t *scratch,size_t scratch_size);
/* Capture BEFORE drawing UI or any OSD; pitch is in bytes, RGB565. */
int spmp_menu_capture_clean(spmp_game_menu *m,const uint16_t *frame,
 unsigned width,unsigned height,size_t pitch_bytes);
/* Returns: 0 no action, 1 menu opened/closed, 2 saved, 3 loaded,
 * 4 exited, -1 storage or unsupported-state failure. */
int spmp_menu_input(spmp_game_menu *m,uint32_t normalized_keys,uint32_t milliseconds);
/* Re-read persisted thumbnail when slot changes, validate CRC. */
int spmp_menu_refresh_preview(spmp_game_menu *m);
int spmp_menu_save(spmp_game_menu *m);
int spmp_menu_load(spmp_game_menu *m);
/* Adapter to Stage9/10 running-core manager; registered core must expose
 * savestate callbacks (currently FCEUmm and Gambatte). */
spmp_ui_emulator spmp_game_core_emulator(void);
int spmp_menu_save_path(const spmp_game_menu *m,char *dst,size_t cap);
void spmp_menu_draw(const spmp_game_menu *m,const spmp_theme *theme,
                    uint16_t *screen,unsigned width,unsigned height);
#endif
