/* SPDX-License-Identifier: MIT
 * SPMP-OS FM-05: UTF-8 text editor, fixed memory, freestanding ARMv5.
 * No filesystem calls in the core. The OS supplies revision-aware I/O.
 */
#ifndef SPMP_FM05_H
#define SPMP_FM05_H
#include "spmp_fm01.h"
#include "spmp_fm04.h"
#define FM05_TEXT_CAP 32768u
#define FM05_SEARCH_CAP 128u
#define FM05_VIEW_ROWS 12u
#define FM05_VIEW_COLS 37u
#define FM05_ERR_TOO_LARGE (-20)
#define FM05_ERR_ENCODING (-21)
#define FM05_ERR_CONFLICT (-22)
#define FM05_ERR_PATH (-23)
#define FM05_ERR_READ_ONLY (-24)
#define FM05_ERR_NO_FILE (-25)
typedef const char *(*fm05_translate)(void *context,unsigned token);
enum { FM05_TX_EDITOR=0,FM05_TX_LINE=1,FM05_TX_COLUMN=2,FM05_TX_BYTES=3,
       FM05_TX_DISCARD=4,FM05_TX_DISCARD_HELP=5,FM05_TX_CONTROLS=6 };
typedef int (*fm05_load)(void *ctx,const char *relative_path,char *out,
                          unsigned cap,unsigned *n,uint64_t *revision);
typedef int (*fm05_write)(void *ctx,const char *relative_path,const char *text,
                          unsigned size,uint64_t expected_revision,uint64_t *updated_revision);
typedef struct {fm05_load read;fm05_write write;void *context;} fm05_fs;
typedef enum {FM05_CLOSED=0,FM05_OPEN=1,FM05_DISCARD_CONFIRM=2} fm05_state;
typedef enum {FM05_INPUT_NONE=0,FM05_INPUT_TEXT=1,FM05_INPUT_SEARCH=2} fm05_input_mode;
typedef struct {
    char text[FM05_TEXT_CAP+1u];
    char path[FM01_PATH_MAX];
    char search[FM05_SEARCH_CAP];
    fm05_fs fs;
    fm05_translate translate;
    void *translation_context;
    uint64_t revision;
    uint32_t length,caret,top_line,goal_col,first_col;
    uint32_t lines;
    uint8_t dirty,crlf,locale,read_only,state,search_wrapped;
    int last_error;
    /* Single-step undo for one insertion/deletion up to 255 bytes. */
    char undo_bytes[256];
    uint32_t undo_pos;
    uint16_t undo_old_size,undo_new_size;
    uint8_t undo_valid;
} fm05_doc;

/* Loading an unsupported/binary/oversized document fails without truncation. */
void fm05_init(fm05_doc *doc,fm05_fs fs);
void fm05_set_translation(fm05_doc *doc,fm05_translate translate,void *context);
const char *fm05_localize(const fm05_doc *doc,unsigned token);
int fm05_open(fm05_doc *doc,const char *relative_path,unsigned locale);
int fm05_open_selected(fm05_doc *doc,const fm01_model *browser);
int fm05_extension_supported(const char *path);
int fm05_utf8_valid(const char *text,unsigned length);
int fm05_insert(fm05_doc *doc,const char *utf8,unsigned n);
int fm05_newline(fm05_doc *doc);
int fm05_backspace(fm05_doc *doc);
int fm05_delete(fm05_doc *doc);
int fm05_undo(fm05_doc *doc);
void fm05_left(fm05_doc *doc);
void fm05_right(fm05_doc *doc);
void fm05_up(fm05_doc *doc);
void fm05_down(fm05_doc *doc);
void fm05_page(fm05_doc *doc,int direction);
void fm05_line_start(fm05_doc *doc);
void fm05_line_end(fm05_doc *doc);
unsigned fm05_caret_line(const fm05_doc *doc);
unsigned fm05_caret_column(const fm05_doc *doc);
unsigned fm05_line_start_offset(const fm05_doc *doc,unsigned line);
int fm05_find(fm05_doc *doc,const char *utf8_query,int wrap);
int fm05_save(fm05_doc *doc);
int fm05_request_close(fm05_doc *doc); /* 1 closed; 0 needs confirmation */
void fm05_confirm_close(fm05_doc *doc,int discard);
int fm05_open_keyboard(fm05_doc *doc,fm04_model *keyboard,fm05_input_mode mode);
int fm05_finish_keyboard(fm05_doc *doc,const fm04_model *keyboard,fm05_input_mode mode);
void fm05_buttons(fm05_doc *doc,uint32_t pressed);
/* Thin UI controller. Buttons are edge-triggered events from the system HAL.
 * Provide the session in OS-owned RAM; do not allocate it on a small ARM stack.
 */
typedef struct {fm05_doc doc;fm04_model keyboard;uint8_t input_mode;} fm05_session;
void fm05_session_init(fm05_session *session,fm05_fs fs);
int fm05_session_open(fm05_session *session,const char *path,unsigned locale);
int fm05_session_buttons(fm05_session *session,uint32_t edge_buttons);
void fm05_session_render(const fm05_session *session,uint16_t *framebuffer,unsigned pitch,
                         const fm01_palette *palette,const fm01_font *system_font);
void fm05_render(const fm05_doc *doc,uint16_t *framebuffer,unsigned pitch,
                 const fm01_palette *colors,const fm01_font *system_font);
#endif
