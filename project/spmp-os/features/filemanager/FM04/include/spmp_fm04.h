/* SPDX-License-Identifier: MIT
 * SPMP-OS FM-04: QVGA UTF-8 on-screen keyboard. Freestanding ARMv5, no heap.
 */
#ifndef SPMP_FM04_H
#define SPMP_FM04_H
#include "spmp_fm01.h"
#include "spmp_fm02.h"
#define FM04_COLS 11u
#define FM04_ROWS 4u
#define FM04_KEYS (FM04_COLS * FM04_ROWS)
typedef enum { FM04_EN=0, FM04_RU=1, FM04_SYMBOLS=2 } fm04_layout;
typedef enum { FM04_CLOSED=0, FM04_EDITING=1, FM04_ACCEPTED=2, FM04_CANCELLED=3 } fm04_state;
typedef enum { FM04_FILENAME=0, FM04_LINE=1 } fm04_mode;
typedef enum { FM04_NO_EVENT=0, FM04_CHANGED=1, FM04_ACCEPT=2, FM04_CANCEL=3, FM04_ERROR=-1 } fm04_event;
enum {FM04_TEXT_RENAME=0,FM04_TEXT_INPUT,FM04_TEXT_ERROR,FM04_TEXT_HINT,
    FM04_TEXT_SHIFT,FM04_TEXT_LANG,FM04_TEXT_SYMBOLS,FM04_TEXT_SPACE,
    FM04_TEXT_LEFT,FM04_TEXT_RIGHT,FM04_TEXT_BACKSPACE,FM04_TEXT_DELETE,
    FM04_TEXT_CLEAR,FM04_TEXT_CANCEL,FM04_TEXT_OK};
typedef const char *(*fm04_translate)(void *context,unsigned token);
typedef struct {
    char value[FM01_NAME_MAX];
    uint16_t bytes;       /* Number of bytes, not UTF-8 codepoints */
    uint16_t cursor;      /* UTF-8 boundary [0..bytes] */
    uint8_t selection;    /* 0..43 */
    uint8_t layout;       /* 0 English, 1 Russian, 2 symbols */
    uint8_t locale;       /* 0 Russian UI, 1 English UI */
    uint8_t shift;
    uint8_t mode;
    uint8_t state;
    int8_t error;
    fm04_translate translate;
    void *translation_context;
    fm02_model *rename_target; /* optional integration, no backend IO in FM04 */
} fm04_model;

/* Return 0 success; reject malformed UTF-8, oversized or prohibited initial names. */
int fm04_open(fm04_model *k, const char *initial, fm04_mode mode, unsigned locale);
int fm04_open_rename(fm04_model *k, fm02_model *actions);
void fm04_set_translation(fm04_model *k,fm04_translate callback,void *context);
const char *fm04_localize(const fm04_model *k,unsigned token); /* FM02_RENAME_INPUT_REQUIRED */
int fm04_utf8_valid(const char *value, unsigned max_bytes);
int fm04_filename_valid(const char *value); /* FAT-friendly filename, exact UTF-8 */
int fm04_insert(fm04_model *k, const char *utf8_one_character);
int fm04_backspace(fm04_model *k);
int fm04_delete(fm04_model *k);
void fm04_move_cursor(fm04_model *k, int direction);
fm04_event fm04_activate(fm04_model *k, unsigned index);
fm04_event fm04_buttons(fm04_model *k, uint32_t pressed);
fm04_event fm04_finish(fm04_model *k);
void fm04_cancel(fm04_model *k);
const char *fm04_key_label(const fm04_model *k, unsigned index);
void fm04_render_overlay(const fm04_model *k, uint16_t *framebuffer, unsigned pitch,
    const fm01_palette *palette, const fm01_font *font);
#endif
