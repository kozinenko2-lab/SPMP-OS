/* SPMP-OS: portable PlayStation BIOS loader / metadata (no game assets). */
#ifndef SPMP_PSX_BIOS_H
#define SPMP_PSX_BIOS_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
#define SPMP_PSX_BIOS_BYTES 524288u
#define SPMP_PSX_SCPH101_V45_CRC32 UINT32_C(0x171bdcec)
typedef enum {
    SPMP_PSX_BIOS_OK=0, SPMP_PSX_BIOS_BAD_ARGUMENT=-1,
    SPMP_PSX_BIOS_PATH_TOO_LONG=-2, SPMP_PSX_BIOS_NOT_FOUND=-3,
    SPMP_PSX_BIOS_WRONG_SIZE=-4, SPMP_PSX_BIOS_READ_FAILED=-5,
    SPMP_PSX_BIOS_INVALID=-6
} spmp_psx_bios_status;
typedef struct {
    uint32_t crc32;
    char region; /* 'A' US / 'E' Europe / 'J' Japan; '?' if unknown */
    int known_scph101_v45;
    char version[48];
} spmp_psx_bios_info;
/* A leading slash in `name` is accepted (legacy PSX4ALL config). */
int spmp_psx_bios_make_path(char *out, size_t cap,const char *dir,const char *name);
/* Zero out `destination` on any error. No retained file descriptors.
 * Other legitimate BIOS versions are accepted if valid; only the supplied
 * SCPH101 v4.5 hash receives the specific known flag. */
spmp_psx_bios_status spmp_psx_bios_load(const char *path, uint8_t *destination,
                                        size_t capacity, spmp_psx_bios_info *info);
const char *spmp_psx_bios_status_text(spmp_psx_bios_status code);
#ifdef __cplusplus
}
#endif
#endif
