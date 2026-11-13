#include "spmp_overlay.h"
#include "spmp_cores.h"
#ifndef SPMP_OVERLAY_CORE_ID
#define SPMP_OVERLAY_CORE_ID "stage10-probe"
#endif
/* Test-only module: intentionally has no NAND/MMIO operations and is NEVER
 * executed from an unverified Defender RAM map. All metadata in ELF32. */
__attribute__((section(".spmp.meta"),used))
const spmp_module_meta spmp_module_info_v1={
 SPMP_OVERLAY_MAGIC, SPMP_OVERLAY_ABI,SPMP_CORE_API_VERSION,5u, 65536u,
 SPMP_OVERLAY_CORE_ID,{0u,0u}
};
static volatile uint32_t counter;
static uint32_t data_words[3]={0x11223344u,0x55667788u,0u};
__attribute__((noinline)) static uint32_t take(uint32_t n){return data_words[n%2u]+n;}
/* GCC / clang emits branch and data relocations for loader verification. */
__attribute__((used)) int spmp_module_entry_v1(const void *api){
 counter+=take(data_words[1]&1u);
 return api? (int)(counter&0x7fffffffu):0;
}
