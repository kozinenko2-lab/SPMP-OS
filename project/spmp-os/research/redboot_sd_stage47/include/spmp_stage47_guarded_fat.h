#ifndef SPMP_STAGE47_GUARDED_FAT_H
#define SPMP_STAGE47_GUARDED_FAT_H
/* Stage47: bounded, read-only integration of proven sector backend -> Stage46
 * guard -> Stage42 FAT -> Stage41 stock fallback. No MMIO, NAND or SD writes.
 * This is not a driver and not a deployable bootloader. */
#include <stdint.h>
#include "spmp_fat_bridge.h"
#include "spmp_stage46_sector_guard.h"
#include "spmp_bootlog.h"

enum spmp47_address_mode { SPMP47_ADDR_BLOCKS=1, SPMP47_ADDR_BYTES=512 };
/* LBA-to-argument conversion is needed by an SD backend, but this function
 * does NOT send commands. Reject overflow explicitly (especially SDSC). */
int spmp47_command_argument(uint32_t lba, enum spmp47_address_mode mode,
                            uint32_t *argument);

typedef struct {
    spmp_fat_bridge_context bridge;  /* FIRST: usable as Stage41 context */
    spmp46_reader trusted_reader;    /* backend MUST be checked on target */
    spmp_bootlog *optional_ram_log;  /* RAM only: never do file IO here */
    uint32_t successful_sectors, failed_sectors;
    uint32_t last_lba, last_code;
    uint32_t verbose;               /* 1: also log successful sectors */
    uint32_t in_sector;             /* defensive recursion guard */
} spmp47_guarded_fat;

/* All callbacks required. physical_sectors MUST come from a verified backend;
 * no guessing capacity from card headers or arbitrary RAM. */
int spmp47_guarded_fat_init(spmp47_guarded_fat *f,
                            const spmp46_reader *reader,
                            uint32_t physical_sectors, uint32_t max_reads,
                            spmp_bootlog *log, uint32_t verbose);
/* Adapter for spmp_fat_io.read_sector(). It keeps the caller's buffer
 * unchanged if the guarded transaction fails. */
int spmp47_guarded_sector(void *context, uint32_t lba, uint8_t out[512]);
/* Compatible with spmp_db_ops.read_package when ops.context=&spmp47_guarded_fat.
 * Other policy callbacks on that context must be provided by the integrator. */
int spmp47_read_package(void *context, const char *path,
                        uint8_t *output, size_t capacity, size_t *actual);
#endif
