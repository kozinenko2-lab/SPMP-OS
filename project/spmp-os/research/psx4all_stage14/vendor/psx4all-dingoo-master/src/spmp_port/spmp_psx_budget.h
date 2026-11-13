#ifndef SPMP_PSX_BUDGET_H
#define SPMP_PSX_BUDGET_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Conservative ESTIMATE of memory required by PSX4ALL static buffers and
 * the rest of the runtime. Does not yet relocate static BSS into SPMP arena. */
typedef struct {
    size_t jit_bytes, ram_bytes, bios_bytes, lut_bytes, vram_bytes;
    size_t spu_budget, core_code_budget, system_budget;
    size_t disc_cache_budget, video_surface_budget, misc_budget;
    size_t total_bytes;
} spmp_psx_budget;
int spmp_psx_budget_plan(size_t ram_capacity, unsigned jit_kb,
                         spmp_psx_budget *out);
#ifdef __cplusplus
}
#endif
#endif
