/* SPMP-OS experimental SD-first / internal-ROM fallback policy.
 * Not a bootloader for flashing. All hardware operations are supplied by a
 * verified platform-specific adapter; never execute on an unverified board.
 * No SD, NAND, MMIO or cache accesses exist in this module.
 */
#ifndef SPMP_DUALBOOT_H
#define SPMP_DUALBOOT_H
#include <stddef.h>
#include <stdint.h>

#define SPMP_DB_HEADER_BYTES 48u
#define SPMP_DB_MAX_IMAGE_BYTES (8u*1024u*1024u)
#define SPMP_DB_DEFAULT_PATH "SPMPOS/BOOT.PKG" /* FAT 8.3 names */

enum spmp_db_event {
    SPMP_DB_EV_FORCE_FACTORY = 1,
    SPMP_DB_EV_NO_CARD_OR_PACKAGE,
    SPMP_DB_EV_INVALID_PACKAGE,
    SPMP_DB_EV_RAM_REJECTED,
    SPMP_DB_EV_STAGE_FAILED,
    SPMP_DB_EV_SD_HANDOFF,
    SPMP_DB_EV_HANDOFF_RETURNED,
    SPMP_DB_EV_FACTORY_HANDOFF,
    SPMP_DB_EV_FACTORY_RESTORE_FAILED
};

typedef struct {
    void *context;
    /* Current board profile must be verified from real hardware, nonzero. */
    uint32_t verified_board_id;
    /* If nonzero, boot factory with no SD access (user emergency key). */
    int (*force_factory)(void *context);
    /* Return 1 only when fully read. Never copy more than capacity. Must
     * have a bounded device-level timeout, including SD init/read errors. */
    int (*read_package)(void *context, const char *path,
                        uint8_t *buffer, size_t capacity, size_t *actual);
    /* Required before attempting SD: restore SD clocks, pin mux and active
     * transport state if fallback is needed, without changing NAND/ROMFS. */
    int (*restore_factory_environment)(void *context);
    /* Must validate exact RAM layout/slot ownership AND valid ELF segments
     * using Stage34 gate or stronger equivalent. Return a proven entry. */
    int (*preflight_elf)(void *context, const uint8_t *elf, size_t length,
                         uint32_t *entry);
    /* Must copy ELF to owned RAM, clean D-cache, invalidate I-cache; return
     * success only after all actions succeed. May change proven entry. */
    int (*stage_elf)(void *context, const uint8_t *elf, size_t length,
                     uint32_t *entry);
    /* Jump is NEVER expected to return. If it does, default to factory. */
    void (*jump_sd)(void *context, uint32_t entry);
    /* Boot the SAME factory ROMFS path as the stock loader. No SD dependency.
     * Never expected to return. Must be fully independent of SD. */
    void (*boot_factory)(void *context);
    void (*on_event)(void *context, enum spmp_db_event event);
} spmp_db_ops;

/* Validated in-memory package, not a trusted/authenticated signed image.
 * Binary little endian header (48 bytes):
 * 0 magic[8]="SPMPDB01"; 8 u32 version=1; 12 u32 header_size=48;
 * 16 u32 payload_length; 20 u32 payload_crc32; 24 u32 board_id;
 * 28 u32 payload_type=1 (ARM ELF32 LE); 32..47 four reserved zero u32.
 * CRC32 detects accidents; it does NOT authenticate image authorship. */
int spmp_db_verify_package(const uint8_t *buffer, size_t n,
                           uint32_t verified_board_id,
                           const uint8_t **elf, size_t *elf_size);
uint32_t spmp_db_crc32(const uint8_t *bytes, size_t len);
/* A return always means that the factory boot callback returned or was
 * unavailable. On a real device successful handoff never returns. */
int spmp_db_boot(const spmp_db_ops *ops, uint8_t *staging, size_t capacity);
#endif
