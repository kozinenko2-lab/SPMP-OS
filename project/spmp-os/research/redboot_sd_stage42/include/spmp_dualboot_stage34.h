/* Optional bridge to pre-existing Stage34 ELF gate. Build only within
 * Stage34 sources. Still requires verified hardware callbacks. */
#ifndef SPMP_DUALBOOT_STAGE34_H
#define SPMP_DUALBOOT_STAGE34_H
#include "spmp_boot_gate.h"
typedef struct {
    const spmp_boot_layout *layout;
    const spmp_boot_platform *platform;
} spmp_db_stage34;
int spmp_db_stage34_preflight(void *ctx, const uint8_t *elf,
                             size_t n, uint32_t *entry);
int spmp_db_stage34_stage(void *ctx, const uint8_t *elf,
                         size_t n, uint32_t *entry);
#endif
