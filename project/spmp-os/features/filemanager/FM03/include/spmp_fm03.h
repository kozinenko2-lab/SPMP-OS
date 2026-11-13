/* SPDX-License-Identifier: MIT
 * SPMP-OS FM03: read-only property sheet and cooperative directory statistics.
 * No heap, libc, filesystem syscalls, or built-in fonts in ARM code.
 */
#ifndef SPMP_FM03_H
#define SPMP_FM03_H
#include "spmp_fm01.h"
#include <stdint.h>
#define FM03_SCAN_BATCH_MAX 64u
#define FM03_MAX_SCAN_DEPTH 16u
#define FM03_MAX_SCAN_ITEMS 250000u
#define FM03_ATTR_READ_ONLY 1u
#define FM03_ATTR_HIDDEN 2u
#define FM03_ATTR_SYSTEM 4u
#define FM03_ATTR_ARCHIVE 8u

typedef struct {
    char name[FM01_NAME_MAX];
    char path[FM01_PATH_MAX]; /* relative to SD root; no leading slash */
    uint64_t bytes;
    uint16_t year;
    uint8_t month, day, hour, minute, second;
    uint8_t kind; /* FM01_ENTRY_DIR / FM01_ENTRY_FILE */
    uint8_t attrs;
} fm03_info;
typedef struct {
    uint64_t total_bytes, free_bytes, available_bytes;
    uint8_t valid;
} fm03_volume;
typedef struct {
    uint64_t bytes;
    uint32_t files, directories, skipped, visited;
} fm03_scan;
/* Backend step: 1 = more work, 0 = finished, negative = error.
 * The adapter owns its iterator resources. budget limits directory entries processed.
 * All callbacks must operate strictly inside the SD root.
 */
typedef struct {
    int (*info)(void*,const char*,const char*,fm03_info*);
    int (*space)(void*,fm03_volume*);
    int (*begin)(void*,const char*,const char*);
    int (*step)(void*,uint32_t,fm03_scan*);
    void (*end)(void*);
} fm03_backend;
typedef enum { FM03_CLOSED=0, FM03_FILE=1, FM03_SCANNING=2,
    FM03_FINISHED=3, FM03_CANCELLED=4, FM03_ERROR=5 } fm03_state;
typedef struct {
    fm01_model *browser;
    fm03_backend ops;
    void *ctx;
    fm03_info info;
    fm03_volume volume;
    fm03_scan scan;
    fm03_state state;
    int last_error;
    uint8_t is_open, has_info, scan_started;
} fm03_model;

void fm03_init(fm03_model*,fm01_model*,const fm03_backend*,void*);
int fm03_open(fm03_model*,const char*directory,const char*name);
int fm03_open_selected(fm03_model*);
int fm03_tick(fm03_model*,uint32_t budget);
void fm03_cancel_scan(fm03_model*);
void fm03_close(fm03_model*);
/* Return 1 if overlay consumed the event, else 0. B exits property sheet;
 * START cancels a directory scan but keeps partial statistics visible. */
int fm03_buttons(fm03_model*,uint32_t pressed);
/* Call before FM-02 and FM-01 button handlers: START+SELECT opens selected properties.
 * When visible, FM-03 consumes navigation keys; B closes, START cancels scan. */
int fm03_dispatch(fm03_model*,uint32_t pressed);
void fm03_render_overlay(const fm03_model*,uint16_t *rgb565,unsigned pitch,
    const fm01_palette*,const fm01_font*);
#endif
