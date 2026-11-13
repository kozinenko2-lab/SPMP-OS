#ifndef DEFOS_H
#define DEFOS_H
#include <stdint.h>
#include <stddef.h>
#include "spmp_theme.h"
#define DEFOS_SCREEN_W 320
#define DEFOS_SCREEN_H 240
#define DEFOS_MAX_ENTRIES 128
#define DEFOS_NAME_MAX 95
#define DEFOS_REL_MAX 255

typedef enum {
    DEFOS_FILE_UNKNOWN=0, DEFOS_FILE_DIR, DEFOS_FILE_NES, DEFOS_FILE_GB,
    DEFOS_FILE_GBA, DEFOS_FILE_SNES, DEFOS_FILE_MD, DEFOS_FILE_PCE,
    DEFOS_FILE_TEXT, DEFOS_FILE_PROGRAM, DEFOS_FILE_BIN,
    DEFOS_FILE_AUDIO, DEFOS_FILE_VIDEO, DEFOS_FILE_IMAGE, DEFOS_FILE_WAD,
    DEFOS_FILE_MSX, DEFOS_FILE_SMS, DEFOS_FILE_ATARI,
    DEFOS_FILE_ZX, DEFOS_FILE_C64, DEFOS_FILE_DISK,
    DEFOS_FILE_TAPE, DEFOS_FILE_SNAPSHOT, DEFOS_FILE_PS1
} defos_kind;
typedef struct {
    char name[DEFOS_NAME_MAX+1];
    defos_kind kind;
    uint64_t size;
} defos_entry;
typedef int (*defos_list_fn)(void *ctx, const char *relative,
                             defos_entry *entries, size_t capacity);
typedef struct {
    char relative[DEFOS_REL_MAX+1];
    char message[96];
    defos_entry entries[DEFOS_MAX_ENTRIES];
    size_t count, selected, scroll;
    defos_list_fn list;
    void *fs_context;
    spmp_theme theme;
} defos_model;

defos_kind defos_classify(const char *name, int is_directory);
const char *defos_kind_name(defos_kind kind);
void defos_init(defos_model *model, defos_list_fn list, void *ctx);
int defos_refresh(defos_model *model);
int defos_enter(defos_model *model);
void defos_back(defos_model *model);
void defos_move(defos_model *model, int change);
void defos_render(const defos_model *model, uint16_t *rgb565);
#endif
