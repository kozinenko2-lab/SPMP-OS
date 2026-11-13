#ifndef SPMP_FAT_READER_H
#define SPMP_FAT_READER_H
#include <stddef.h>
#include <stdint.h>
/* Read-only FAT16/FAT32 file loader. SD hardware / MMC command layer is NOT
 * implemented here. A verified board backend must provide bounded sector
 * reads (512 bytes) and be safe when the card is absent or unresponsive. */
#define SPMP_FAT_SECTOR_BYTES 512u
#define SPMP_FAT_MAX_PACKAGE (8u*1024u*1024u+48u)
#define SPMP_FAT_BOOT_PATH "SPMPOS/BOOT.PKG"
typedef struct {
    void *context;
    uint32_t physical_sectors; /* verified SD capacity, strictly nonzero */
    uint32_t max_reads; /* total budget incl. FAT, MBR and directory sectors */
    int (*read_sector)(void *context, uint32_t lba, uint8_t out[512]);
} spmp_fat_io;
/* Reads exactly SPMPOS/BOOT.PKG into output. Return 1 only for a complete
 * file. On error, *actual=0 and any copied output prefix is zeroed. Never
 * writes to SD, Flash, any MMIO, or unspecified memory. */
int spmp_fat_read_boot(const spmp_fat_io *io, uint8_t *output,
                       size_t capacity, size_t *actual);
#endif
