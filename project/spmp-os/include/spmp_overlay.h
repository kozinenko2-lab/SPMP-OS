/* SPMP-OS Stage 10: freestanding-compatible, inspectable ARMv5 ELF32 ET_REL
 * overlay loader. Not a RedBoot/FSP image and not a device-tested loader.
 * No MMIO, NAND, dynamic allocations, or host libc in this interface.
 */
#ifndef SPMP_OVERLAY_H
#define SPMP_OVERLAY_H
#include <stddef.h>
#include <stdint.h>
#define SPMP_OVERLAY_ABI 1u
#define SPMP_OVERLAY_MAGIC 0x504d5053u /* bytes "SPMP" in little endian */
#define SPMP_OVERLAY_MAX_SECTIONS 256u
#define SPMP_OVERLAY_ID_LEN 24u
#define SPMP_OVERLAY_MAX_BYTES (16u * 1024u * 1024u)

typedef struct {
 uint32_t magic, abi_version, core_api_version, target_arm_isa, max_game_bytes;
 char core_id[SPMP_OVERLAY_ID_LEN];
 uint32_t reserved[2];
} spmp_module_meta; /* fixed 52-byte layout; ARM EABI + host C */

typedef enum {
 SPMP_OV_OK=0, SPMP_OV_BAD_INPUT=-1, SPMP_OV_BAD_ELF=-2,
 SPMP_OV_OVERFLOW=-3, SPMP_OV_NO_MEMORY=-4,
 SPMP_OV_BAD_META=-5, SPMP_OV_BAD_SYMBOL=-6,
 SPMP_OV_BAD_RELOCATION=-7, SPMP_OV_UNSUPPORTED=-8,
 SPMP_OV_NOT_READY=-9
} spmp_ov_status;

typedef void (*spmp_cache_callback)(void *ctx, void *slot, size_t bytes);
typedef struct {
 uint32_t virtual_base;  /* DEVICE VMA chosen only after verified RAM map */
 spmp_cache_callback dcache_clean;
 spmp_cache_callback icache_invalidate;
 void *cache_ctx;
} spmp_overlay_options;

typedef struct {
 spmp_module_meta meta;
 uint32_t entry_vma;
 size_t used_bytes;
 uint32_t loaded_sections;
 uint32_t relocation_count;
 uint32_t source_fingerprint; /* cache key, NOT signature/authenticity */
 int executable; /* 1 ONLY if BOTH cache callbacks were called */
} spmp_overlay_image;

/* Dry-run validates ALL relocation targets and branch ranges; does not write
 * to code RAM. The ELF bytes must not overlap the output RAM slot. */
int spmp_overlay_probe(const uint8_t *elf, size_t elf_size,
 size_t slot_size, const spmp_overlay_options *options,
 spmp_overlay_image *out);
int spmp_overlay_load(const uint8_t *elf, size_t elf_size,
 void *slot, size_t slot_size, const spmp_overlay_options *options,
 spmp_overlay_image *out);

/* One reusable overlay slot. Caller must first stop running code and persist
 * saves before switch. A rejected probe preserves the resident module. */
typedef struct {
 uint8_t *slot;
 size_t slot_size;
 spmp_overlay_options options;
 spmp_overlay_image resident;
 int loaded;
 uint32_t generations;
} spmp_overlay_manager;
void spmp_overlay_manager_init(spmp_overlay_manager *mgr, void *ram,
 size_t bytes, const spmp_overlay_options *options);
int spmp_overlay_manager_switch(spmp_overlay_manager *mgr,
 const uint8_t *elf, size_t bytes, spmp_overlay_image *out);
void spmp_overlay_manager_unload(spmp_overlay_manager *mgr);
const char *spmp_overlay_error(int status);
#endif
