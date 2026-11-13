/* SPDX-License-Identifier: MIT
 * SPMP-OS File Manager FM-01: read-only two-pane browsing model and QVGA renderer.
 * No system allocator or FAT dependency in ARM core. Filesystem API is pluggable.
 */
#ifndef SPMP_FM01_H
#define SPMP_FM01_H
#include <stddef.h>
#include <stdint.h>

#define FM01_PATH_MAX 512u
#define FM01_NAME_MAX 256u
#define FM01_PAGE_ROWS 9u
#define FM01_MAX_DEPTH 16u
#define FM01_WIDTH 320u
#define FM01_HEIGHT 240u
#define FM01_ENTRY_DIR 1u
#define FM01_ENTRY_FILE 2u

typedef struct {
    char name[FM01_NAME_MAX]; /* UTF-8, never contains slash */
    uint64_t size_bytes;
    uint16_t year;
    uint8_t month, day, type;
    uint8_t read_only;
} fm01_entry;

/* Fetch an indexed page in stable order (directories first, then UTF-8 byte order).
 * The filesystem adapter must return matching total/file/dir counts every time.
 * A page can contain 0..capacity records. status 0 success, nonzero error.
 * Unix/backend readdir and FAT dir iteration belong OUTSIDE the ARM model.
 */
typedef int (*fm01_list_page)(void *context, const char *relative_path,
    uint32_t offset, uint32_t capacity, fm01_entry *entries,
    uint32_t *returned, uint32_t *total, uint32_t *file_count,
    uint32_t *dir_count);

typedef struct {
    char path[FM01_PATH_MAX];
    fm01_entry entries[FM01_PAGE_ROWS];
    uint32_t total, file_count, dir_count;
    uint32_t cursor, first, loaded;
    uint32_t saved_cursor[FM01_MAX_DEPTH];
    uint8_t depth;
    int last_error;
} fm01_panel;

typedef struct {
    fm01_panel panels[2];
    fm01_list_page list;
    void *fs_context;
    uint8_t active;
    uint8_t locale; /* 0=Russian, 1=English; UI texts are NEVER baked in icons */
    uint8_t sd_present;
} fm01_model;

enum {
    FM01_KEY_UP=1u<<0, FM01_KEY_DOWN=1u<<1,
    FM01_KEY_LEFT=1u<<2, FM01_KEY_RIGHT=1u<<3,
    FM01_KEY_A=1u<<4, FM01_KEY_B=1u<<5,
    FM01_KEY_START=1u<<6, FM01_KEY_SELECT=1u<<7
};

typedef struct {
    uint16_t background, panel, panel_inactive, line, text, secondary,
             active_border, highlight, highlight_text, footer;
} fm01_palette;

typedef void (*fm01_font_draw)(void *ctx, uint16_t *rgb565,
    unsigned pitch_pixels, unsigned x, unsigned y, const char *text,
    uint16_t rgb565_color, unsigned max_width, unsigned font_pixels);

typedef struct {
    fm01_font_draw draw;
    void *ctx;
} fm01_font;

typedef struct {
    char time_hhmm[6]; /* supplied by actual RTC/platform, e.g. 11:42 */
    unsigned volume_0_100;
    int sd_present;
} fm01_status;

void fm01_init(fm01_model *model, fm01_list_page list, void *context);
int fm01_refresh(fm01_model *model, unsigned which);
void fm01_buttons(fm01_model *model, uint32_t pressed);
int fm01_enter(fm01_model *model);
int fm01_up(fm01_model *model);
void fm01_select_panel(fm01_model *model, unsigned which);
const fm01_entry *fm01_selected(const fm01_model *model, unsigned which);
void fm01_palette_default(fm01_palette *palette);
void fm01_render(const fm01_model *model, uint16_t *framebuffer,
    unsigned pitch_pixels, const fm01_palette *colors,
    const fm01_font *font, const fm01_status *status);
/* Overlay preserves a theme background already rendered into the same framebuffer.
 * No second framebuffer or extra 153.6 KB buffer is allocated. */
void fm01_render_overlay(const fm01_model *model, uint16_t *framebuffer,
    unsigned pitch_pixels, const fm01_palette *colors,
    const fm01_font *font, const fm01_status *status);
void fm01_size_text(uint64_t bytes, char *dst, size_t size);
void fm01_date_text(const fm01_entry *entry, char *dst, size_t size);
#endif
