/* SPMP-OS Stage 34 — gated RAM overlay preparation.
 * This is NOT a RedBoot executable, a flash updater, or a way to discover RAM.
 * The supplied layout and host callbacks MUST come from separately verified,
 * platform-specific code before this API may ever be used on hardware.
 */
#ifndef SPMP_BOOT_GATE_H
#define SPMP_BOOT_GATE_H
#include <stddef.h>
#include <stdint.h>
#include "spmp_overlay.h"
#define SPMP_BOOT_GATE_ABI 1u
#define SPMP_BOOT_MAX_RESERVED 12u

typedef struct { uint32_t start, bytes; } spmp_boot_range;
typedef struct {
    /* All of these addresses come from a device-specific measured memory map.
     * They MUST NOT be inferred from generic RedBoot sample configurations. */
    spmp_boot_range ram;
    spmp_boot_range module_slot;
    spmp_boot_range reserved[SPMP_BOOT_MAX_RESERVED];
    unsigned reserved_count;
    uint32_t board_revision;   /* measured, nonzero; not a cryptographic proof */
} spmp_boot_layout;

typedef struct {
    void *context;
    /* Verifies OWNERSHIP of this exact RAM slot at staging time. A successful
     * geometry check alone is NOT proof the slot is free on a live device. */
    int (*confirm_slot_owned)(void *context, uint32_t start, uint32_t bytes);
    /* Returns a writable mapping for precisely the confirmed slot. */
    void *(*map_slot)(void *context, uint32_t start, uint32_t bytes);
    /* Cache callbacks MUST report success; no safe fallback is assumed. */
    int (*clean_dcache)(void *context, void *address, size_t bytes);
    int (*invalidate_icache)(void *context, void *address, size_t bytes);
} spmp_boot_platform;

typedef enum {
    SPMP_BOOT_OK = 0,
    SPMP_BOOT_BAD_INPUT = -101,
    SPMP_BOOT_BAD_MAP = -102,
    SPMP_BOOT_NOT_ATTESTED = -103,
    SPMP_BOOT_UNMAPPED = -104,
    SPMP_BOOT_UNSAFE_ALIAS = -105,
    SPMP_BOOT_CACHE_FAILED = -106,
    SPMP_BOOT_OVERLAY_ERROR = -107
} spmp_boot_status;

typedef struct {
    spmp_overlay_image overlay;
    spmp_boot_range slot;
    /* Staged does NOT mean executed. External platform launch remains absent. */
    int staged;
    int cache_confirmed;
} spmp_boot_result;

/* Pure checks: no memory writes, no MMIO, no callback invocation. */
int spmp_boot_validate_layout(const spmp_boot_layout *layout);
int spmp_boot_preflight(const spmp_boot_layout *layout, const uint8_t *elf,
    size_t elf_bytes, spmp_boot_result *result);

/* On a real board: DANGEROUS until a vetted implementation of the platform
 * callbacks exists. Never use pointers fabricated from physical addresses.
 * The only side effects are writes to a caller-confirmed RAM slot and caches. */
int spmp_boot_stage(const spmp_boot_layout *layout,
    const spmp_boot_platform *platform, const uint8_t *elf,
    size_t elf_bytes, spmp_boot_result *result);
const char *spmp_boot_status_text(int code);
#endif
