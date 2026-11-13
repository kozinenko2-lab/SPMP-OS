#ifndef SPMP_UI_THEME_V7_H
#define SPMP_UI_THEME_V7_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Pure ARMv5/RGB565 renderer: no filesystem, malloc, libc, Linux or RedBoot calls. */
#define SPMP_UI_WIDTH 320u
#define SPMP_UI_HEIGHT 240u
#define SPMP_UI_ROW_BYTES 640u
#define SPMP_UI_BG_BYTES 153616u
#define SPMP_UI_BG_SCRATCH 2560u
#define SPMP_UI_MAX_ITEMS 32u
#define SPMP_UI_SOUND_FRAMES 640u
#define SPMP_UI_SOUND_HZ 16000u
#define SPMP_UI_LAYOUT_LIST 0u
#define SPMP_UI_LAYOUT_GRID 1u

typedef struct {
 uint16_t accent, panel, text, muted, selection, border;
 uint8_t layout_default, enable_sfx;
} spmp_ui_style;
typedef struct {
 const char *label;   /* UTF-8, caller-owned; Cyrillic supported */
 uint8_t icon;        /* 0 games,1 music,2 video,3 photos,4 radio,5 files,6 settings,7 themes */
} spmp_ui_item;
typedef struct {
 uint8_t hour, minute, volume_percent, sd_present;
} spmp_ui_status;
typedef struct {
 uint8_t layout,selected,scroll,menu_count;
 uint16_t frame, anim_frame;
 uint8_t previous_selected;
} spmp_ui_state;
typedef enum {
 SPMP_UI_UP=1,SPMP_UI_DOWN=2,SPMP_UI_LEFT=3,SPMP_UI_RIGHT=4,
 SPMP_UI_ACCEPT=5,SPMP_UI_BACK=6,SPMP_UI_TOGGLE_LAYOUT=7
} spmp_ui_key;
typedef enum {SPMP_UI_SFX_NONE=0,SPMP_UI_SFX_MOVE=1,SPMP_UI_SFX_ACCEPT=2,SPMP_UI_SFX_BACK=3} spmp_ui_sfx;
/* read_at fills exactly len bytes from a theme background stored on SD/ROMFS. */
typedef int (*spmp_ui_read_at)(void *ctx,uint32_t offset,void *dst,uint32_t len);

int spmp_ui_parse_style(const uint8_t *data,size_t len,spmp_ui_style *style);
int spmp_ui_validate_font(const uint8_t *data,size_t len);
int spmp_ui_validate_icons(const uint8_t *data,size_t len);
int spmp_ui_validate_background(spmp_ui_read_at read,void *ctx);
void spmp_ui_state_init(spmp_ui_state *state,uint8_t layout,uint8_t item_count);
spmp_ui_sfx spmp_ui_key_event(spmp_ui_state *state,spmp_ui_key key);
void spmp_ui_animate(spmp_ui_state *state); /* call on UI tick, ~50 ms */
/* Existing 320x240 framebuffer supplied by host; scratch can be just 2560 bytes. */
int spmp_ui_render(uint16_t *fb,unsigned pitch_pixels,
                   spmp_ui_read_at bg_read,void *bg_ctx,
                   uint8_t *scratch,size_t scratch_len,
                   const spmp_ui_style *style,
                   const uint8_t *font,size_t font_len,
                   const uint8_t *icons,size_t icons_len,
                   const spmp_ui_item *items,unsigned count,
                   const spmp_ui_status *status,
                   const spmp_ui_state *state);
/* Transparent UI layer: draw onto existing RGB565 framebuffer (no wallpaper IO). */
int spmp_ui_render_overlay(uint16_t *fb,unsigned pitch_pixels,
                   const spmp_ui_style *style,
                   const uint8_t *font,size_t font_len,
                   const uint8_t *icons,size_t icons_len,
                   const spmp_ui_item *items,unsigned count,
                   const spmp_ui_status *status,const spmp_ui_state *state);
/* Returns stereo 16-bit samples for user-key clicks; route via existing audio mixer. */
size_t spmp_ui_sfx_generate(spmp_ui_sfx sound,int16_t *stereo,size_t stereo_frames);
#ifdef __cplusplus
}
#endif
#endif
