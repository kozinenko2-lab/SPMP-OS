#ifndef SPMP_STAGE40_FILE_UI_H
#define SPMP_STAGE40_FILE_UI_H
#include <stddef.h>
#include <stdint.h>
#include "spmp_ro_fs.h"
#include "spmp_stage39_lcd_pcm.h"

/* 16-bit values are opaque byte-pairs. No OEM RGB565 claim or direct LCD/MMIO. */
typedef struct { uint8_t first, second; } spmp40_pixel16;
typedef struct {
    spmp40_pixel16 background, panel, active_panel, selection, text, dim_text, border;
} spmp40_theme;
/* System-font rasterizer supplies pixels relative to text origin. It gets no
 * raw framebuffer pointer. plot() clips every requested pixel into its box. */
typedef void (*spmp40_plot)(void *ctx, uint32_t x, uint32_t y);
typedef void (*spmp40_system_font)(void *ctx, const char *utf8,
                                  uint32_t max_width, spmp40_plot plot,
                                  void *plot_context);
typedef struct {
    spmp_ro_dual_panel *dual;
    const spmp_ro_fs *fs;
    /* Separate buffer: avoids multi-kilobyte stack allocations on ARM. */
    spmp_ro_panel *scratch;
} spmp40_file_ui;
typedef enum {
    SPMP40_NAV_UP=1, SPMP40_NAV_DOWN, SPMP40_NAV_SWITCH,
    SPMP40_NAV_ENTER, SPMP40_NAV_PARENT,
    SPMP40_NAV_PAGE_UP, SPMP40_NAV_PAGE_DOWN
} spmp40_nav;

int spmp40_init(spmp40_file_ui *ui, spmp_ro_dual_panel *dual,
                const spmp_ro_fs *fs, spmp_ro_panel *scratch,
                const char *left, const char *right);
/* Does not modify FAT, files, settings, RedBoot, or Flash. */
int spmp40_navigate(spmp40_file_ui *ui, spmp40_nav nav);
/* Standard layout 320x240 in caller-owned software RAM, *no present*. */
int spmp40_render(const spmp40_file_ui *ui,
                  const spmp39_surface16 *surface,
                  const spmp40_theme *theme,
                  spmp40_system_font font, void *font_context);
#endif
