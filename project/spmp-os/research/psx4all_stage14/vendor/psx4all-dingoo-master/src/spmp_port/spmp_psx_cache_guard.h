#ifndef SPMP_PSX_CACHE_GUARD_H
#define SPMP_PSX_CACHE_GUARD_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Cache hook is privileged device HAL code. NEVER call CP15 from a user-mode
 * process. Memory mapping and core mode must be verified by platform startup. */
typedef struct {
    void (*dcache_clean_line)(uintptr_t);
    void (*write_buffer_drain)(void);
    void (*icache_invalidate_line)(uintptr_t);
    void (*prefetch_flush)(void);
} spmp_psx_cache_ops;
typedef struct {
    uintptr_t first, limit; /* verified executable JIT region [first,limit) */
    uint32_t line_bytes;
    unsigned sync_calls;
    unsigned rejected;
    int fault;
    spmp_psx_cache_ops ops;
} spmp_psx_cache_guard;
/* explicit platform approval: code must run privileged and memory map verified;
 * this flag is an integration assertion, not automatic hardware discovery. */
int spmp_psx_cache_guard_attach(spmp_psx_cache_guard *guard,
    uintptr_t start, size_t bytes, unsigned verified_privileged,
    const spmp_psx_cache_ops *ops);
void spmp_psx_cache_guard_detach(spmp_psx_cache_guard *guard);
int spmp_psx_cache_guard_faulted(const spmp_psx_cache_guard *guard);
int spmp_psx_cache_guard_verified_range(uintptr_t first, size_t size);
void spmp_psx_cache_guard_reset_binding(void);
/* Architecture-specific implementation: guarded ARMv5 module, not invoked by host.
 * Returns NULL unless explicitly compiled with the privileged build opt-in. */
const spmp_psx_cache_ops *spmp_psx_arm926_privileged_ops(void);
#ifdef __cplusplus
}
#endif
#endif
