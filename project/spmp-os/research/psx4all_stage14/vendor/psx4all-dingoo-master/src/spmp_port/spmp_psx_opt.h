/* SPMP-OS / ARM926EJ-S adaptation. Public API; no Linux dependencies. */
#ifndef SPMP_PSX_OPT_H
#define SPMP_PSX_OPT_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Correct modulo-2^32 cycle scaling with no software floating point. */
static inline uint32_t spmp_psx_scaled_cycles(uint32_t cycles, uint32_t multiplier) {
    return cycles * multiplier;
}
/* MIPS32 BIOS abs/labs: wrap INT_MIN modulo 2^32, avoid host long width. */
static inline uint32_t spmp_psx_bios_abs32(uint32_t value) {
    return (value & 0x80000000u) ? (0u-value) : value;
}
/* Cache validity is based on ACTUAL sectors read; handles unsigned wrap. */
static inline int spmp_psx_cd_cache_hit(uint32_t sector, uint32_t first, unsigned count) {
    return sector>=first && (sector-first)<count;
}
/* Coordinate-safe fast path: 1 = handled; 0 = unsupported rectangle, fall back. */
int spmp_psx_gpu_move_16(uint16_t *vram, unsigned width, unsigned height,
                         int sx, int sy, int dx, int dy, unsigned w, unsigned h);
int spmp_psx_gpu_clear_16(uint16_t *vram, unsigned width, unsigned height,
                          int x, int y, unsigned w, unsigned h, uint16_t color);
/* Bounded GPU DMA linked-list walk. PS1 uses 2 MiB RAM mirrors.
 * Negative return means corrupt / cyclic list. */
typedef void (*spmp_psx_gpu_cmd_sink)(uint32_t word, void *user);
int spmp_psx_gpu_dma_walk(const uint32_t *psx_ram, unsigned ram_bytes,
                          uint32_t first, unsigned max_nodes,
                          spmp_psx_gpu_cmd_sink sink, void *user);
/* The SPMP-OS HAL MUST bind an implementation performing D-cache clean,
 * write-buffer drain, I-cache invalidate, and prefetch flush as required
 * for ARM926EJ-S, BEFORE activating the JIT. These callbacks are NOT
 * implemented by the SPMP-OS PC simulator. */
typedef void (*spmp_psx_icache_fn)(uintptr_t begin, uintptr_t end, void *user);
int spmp_psx_bind_icache(spmp_psx_icache_fn callback, void *user);
int spmp_psx_icache_ready(void);
void spmp_psx_unbind_icache(void);
void spmp_psx_icache_sync(uint32_t begin, uint32_t end, uint32_t flags);
#ifdef __cplusplus
}
#endif
#endif
