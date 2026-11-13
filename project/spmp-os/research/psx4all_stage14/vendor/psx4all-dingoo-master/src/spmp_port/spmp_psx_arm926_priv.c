#include "spmp_psx_cache_guard.h"
/* ARM926EJ-S CP15 cache-maintenance instructions (ARM DDI 0198E).
 * These operations are privileged. The caller must verify CPU mode and MMU
 * cacheability and know the executable JIT VA range before attaching.
 * Do NOT use when running as an unprivileged eCos/Linux application. */
#if defined(__arm__) && defined(SPMP_PSX_ARM926_PRIVILEGED_CACHE)
static void dclean(uintptr_t mva) {
    __asm__ volatile("mcr p15, 0, %0, c7, c10, 1" :: "r"(mva) : "memory");
}
static void drain(void) {
    unsigned zero=0;
    __asm__ volatile("mcr p15, 0, %0, c7, c10, 4" :: "r"(zero) : "memory");
}
static void iinvalidate(uintptr_t mva) {
    __asm__ volatile("mcr p15, 0, %0, c7, c5, 1" :: "r"(mva) : "memory");
}
static void prefetch(void) {
    unsigned zero=0;
    __asm__ volatile("mcr p15, 0, %0, c7, c5, 4" :: "r"(zero) : "memory");
}
static const spmp_psx_cache_ops hardware_ops={dclean,drain,iinvalidate,prefetch};
const spmp_psx_cache_ops *spmp_psx_arm926_privileged_ops(void) {return &hardware_ops;}
#else
const spmp_psx_cache_ops *spmp_psx_arm926_privileged_ops(void) {return 0;}
#endif
