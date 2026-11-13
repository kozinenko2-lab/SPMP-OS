/* SPDX-License-Identifier: MIT
 * SPMP-OS FM-02: transactional file actions and bounded multi-selection.
 * Hardware-independent. All FAT/SD operations supplied by a verified backend.
 */
#ifndef SPMP_FM02_H
#define SPMP_FM02_H
#include "spmp_fm01.h"
#define FM02_MARK_LIMIT 64u
#define FM02_IO_CHUNK 8192u

typedef enum { FM02_COPY=1, FM02_MOVE=2, FM02_RENAME=3, FM02_DELETE=4 } fm02_action;
typedef enum { FM02_IDLE, FM02_MENU, FM02_RENAME_INPUT_REQUIRED, FM02_CONFIRM_DELETE, FM02_BUSY, FM02_DONE, FM02_FAILED, FM02_CANCELLED } fm02_state;
typedef struct {
    fm02_action action;
    char src_directory[FM01_PATH_MAX];
    char dst_directory[FM01_PATH_MAX];
    char name[FM01_NAME_MAX];
    char new_name[FM01_NAME_MAX]; /* Only for rename */
} fm02_request;
typedef struct {
    uint64_t bytes_done, bytes_total;
    uint32_t items_done, items_total;
    uint8_t phase; /* 1=scan, 2=copy, 3=commit, 4=remove */
} fm02_progress;
typedef int (*fm02_progress_hook)(void *context,const fm02_progress *progress);
typedef int (*fm02_poll_cancel)(void *context); /* non-blocking: true when B pressed */
typedef int (*fm02_backend_action)(void *context,const fm02_request *request,
    fm02_progress_hook progress,void *progress_context,fm02_progress *result);
typedef struct {
    fm01_model *browser;
    fm02_backend_action backend;
    void *backend_context;
    fm02_poll_cancel poll_cancel;
    void *poll_context;
    char mark_directory[FM01_PATH_MAX];
    char marks[FM02_MARK_LIMIT][FM01_NAME_MAX];
    char rename_name[FM01_NAME_MAX];
    uint32_t marks_count;
    uint8_t marks_panel, confirmation, cancel_requested, menu_action;
    fm02_state state;
    fm02_action last_action;
    int last_error;
    uint32_t processed, requested;
    fm02_progress progress;
} fm02_model;

void fm02_init(fm02_model *m,fm01_model *browser,
    fm02_backend_action backend,void *backend_context);
int fm02_toggle_mark(fm02_model *m); /* 1 selected, 0 unselected, negative error */
void fm02_clear_marks(fm02_model *m);
int fm02_is_marked(const fm02_model *m,const char *directory,const char *name,unsigned which);
int fm02_prepare(fm02_model *m,fm02_action action,const char *new_name);
int fm02_commit(fm02_model *m,int confirmed);
void fm02_cancel(fm02_model *m);
void fm02_set_cancel_poll(fm02_model *m,fm02_poll_cancel poll,void *context);
int fm02_supply_rename(fm02_model *m,const char *new_name); /* future FM-04 keyboard */
void fm02_buttons(fm02_model *m,uint32_t pressed);
int fm02_valid_name(const char *name);
void fm02_render_status(const fm02_model *m,uint16_t *rgb565,unsigned pitch,
    const fm01_palette *palette,const fm01_font *font);
#endif
