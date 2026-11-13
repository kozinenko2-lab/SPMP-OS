/* SPMP-OS PSX4ALL: one owned, cache-line-aligned allocation for ARM JIT data.
 * This header intentionally avoids libc and C++ runtime dependencies.
 * Allocation callbacks are supplied by SPMP-OS or the host test harness.
 * Copyright 2026 SPMP-OS contributors; SPDX-License-Identifier: GPL-2.0-or-later.
 */
#ifndef SPMP_PSX_JIT_STORE_H
#define SPMP_PSX_JIT_STORE_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define SPMP_PSX_JIT_RAM_BYTES (2u * 1024u * 1024u)
#define SPMP_PSX_JIT_ROM_BYTES (512u * 1024u)
#define SPMP_PSX_JIT_LUT_BYTES (65536u * sizeof(uint32_t))
#define SPMP_PSX_JIT_ALIGNMENT 64u

typedef void *(*spmp_psx_jit_alloc_fn)(size_t bytes, void *opaque);
typedef void (*spmp_psx_jit_free_fn)(void *address, void *opaque);
typedef struct spmp_psx_jit_store {
    void *allocation;
    uint8_t *code;
    int8_t *ram;
    int8_t *rom;
    uint32_t *lut;
    size_t bytes_owned;
    size_t code_capacity;
} spmp_psx_jit_store;

/* Exported by ARM recompiler, not by this allocator module. HAL must call
 * prepare, validate the returned executable region, then attach cache guard,
 * before calling psxRec.Init. On failure call abort. */
int spmp_psx_rec_prepare(spmp_psx_jit_alloc_fn alloc,
    spmp_psx_jit_free_fn release, void *opaque,
    uintptr_t *address, size_t *length);
void spmp_psx_rec_abort(void);

/* Capacity is 1-8 MiB, multiple of 1024 bytes. No partial success.
 * Caller must provide an empty store (zero-filled struct) and must pair init
 * with release using the SAME allocator context. This is not thread safe.
 */
int spmp_psx_jit_store_init(spmp_psx_jit_store *store, size_t capacity,
                            spmp_psx_jit_alloc_fn alloc, void *opaque);
void spmp_psx_jit_store_release(spmp_psx_jit_store *store,
                                spmp_psx_jit_free_fn release, void *opaque);
#ifdef __cplusplus
}
#endif
#endif
