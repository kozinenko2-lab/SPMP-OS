#include "spmp_psx_jit_store.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>

static uintptr_t aligned_up(uintptr_t value, uintptr_t align) {
    return (value + align - 1u) & ~(align - 1u);
}
int spmp_psx_jit_store_init(spmp_psx_jit_store *store, size_t capacity,
                            spmp_psx_jit_alloc_fn alloc, void *opaque) {
    size_t length;
    uint8_t *p;
    void *allocation;
    if (!store || !alloc || store->allocation || store->code || store->lut ||
        store->ram || store->rom || store->bytes_owned ||
        capacity < 1024u*1024u || capacity > 8u*1024u*1024u ||
        (capacity & 1023u)) return 0;
    /* Include alignment slack for the returned allocation. */
    if (capacity > SIZE_MAX - SPMP_PSX_JIT_RAM_BYTES - SPMP_PSX_JIT_ROM_BYTES -
                   SPMP_PSX_JIT_LUT_BYTES - SPMP_PSX_JIT_ALIGNMENT) return 0;
    length = capacity + SPMP_PSX_JIT_RAM_BYTES + SPMP_PSX_JIT_ROM_BYTES +
             SPMP_PSX_JIT_LUT_BYTES + SPMP_PSX_JIT_ALIGNMENT;
    allocation = alloc(length, opaque);
    if (!allocation) return 0;
    if ((uintptr_t)allocation > UINTPTR_MAX - SPMP_PSX_JIT_ALIGNMENT) return 0;
    p = (uint8_t *)aligned_up((uintptr_t)allocation, SPMP_PSX_JIT_ALIGNMENT);
    /* Memory is private to this core: no allocations survive release. */
    memset(p, 0, length - (size_t)(p - (uint8_t *)allocation));
    store->allocation = allocation;
    store->code = p;
    store->ram = (int8_t *)(p + capacity);
    store->rom = (int8_t *)(p + capacity + SPMP_PSX_JIT_RAM_BYTES);
    store->lut = (uint32_t *)(void *)(p + capacity + SPMP_PSX_JIT_RAM_BYTES +
                                     SPMP_PSX_JIT_ROM_BYTES);
    store->bytes_owned = length;
    store->code_capacity = capacity;
    return 1;
}
void spmp_psx_jit_store_release(spmp_psx_jit_store *store,
                                spmp_psx_jit_free_fn release, void *opaque) {
    if (!store) return;
    if (store->allocation && release) release(store->allocation, opaque);
    memset(store, 0, sizeof(*store));
}
