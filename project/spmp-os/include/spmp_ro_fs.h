/* Read-only, injected FAT/SD facade for SPMP-OS file manager Stage34.
 * All directory operations go through trusted callbacks; no write methods.
 * UTF-8 names are preserved for the system font; no built-in glyph assets. */
#ifndef SPMP_RO_FS_H
#define SPMP_RO_FS_H
#include <stddef.h>
#include <stdint.h>
#define SPMP_FS_PATH_MAX 240u
#define SPMP_FS_NAME_MAX 128u
#define SPMP_FS_MAX_ENTRIES 64u
typedef struct {
    char name[SPMP_FS_NAME_MAX];
    uint64_t bytes;
    uint16_t fat_date; /* raw DOS/FAT date: Y(7):M(4):D(5) */
    uint8_t is_dir;
} spmp_ro_entry;
typedef struct {
    void *context;
    int (*confirm_readonly)(void *context);
    /* Read up to capacity entries, setting *written, without modifications. */
    int (*read_directory)(void *context,const char *absolute_utf8_path,
        size_t skip,spmp_ro_entry *entries,size_t capacity,
        size_t *written,size_t *total_entries,size_t *total_files);
} spmp_ro_fs;
typedef struct {
    char path[SPMP_FS_PATH_MAX];
    spmp_ro_entry entries[SPMP_FS_MAX_ENTRIES];
    size_t count,selected,page_offset,total_entries,total_files;
} spmp_ro_panel;
typedef struct {
    spmp_ro_panel panels[2];
    unsigned active;
} spmp_ro_dual_panel;
/* No relative roots, backslash, traversal, or repeated slashes. */
int spmp_ro_path_valid(const char *path);
/* `scratch` is a separate, caller-owned panel used for staging. Never put a
 * 8+ KB temporary on the small ARM task stack. Rejected listings preserve
 * the visible panel unchanged. Offset is the page to read (0 = first page). */
int spmp_ro_open_page(const spmp_ro_fs *fs,spmp_ro_panel *panel,
    spmp_ro_panel *scratch,const char *path,size_t offset);
int spmp_ro_switch(spmp_ro_dual_panel *dual,unsigned new_active);
int spmp_ro_select(spmp_ro_dual_panel *dual,size_t index);
/* Fat date formatter: example 11.11.26; returns error on invalid fields. */
int spmp_ro_short_date(uint16_t fat_date,char out[9]);
#endif
