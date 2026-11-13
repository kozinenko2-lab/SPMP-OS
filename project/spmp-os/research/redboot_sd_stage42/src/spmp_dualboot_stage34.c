#include "spmp_dualboot_stage34.h"
int spmp_db_stage34_preflight(void *ctx, const uint8_t *elf,
                             size_t n, uint32_t *entry) {
    spmp_db_stage34 *g=(spmp_db_stage34*)ctx;
    spmp_boot_result r;
    if(entry) *entry=0;
    if(!g || !g->layout || !elf || !entry) return 0;
    if(spmp_boot_preflight(g->layout,elf,n,&r)!=SPMP_BOOT_OK)return 0;
    if(!r.overlay.entry_vma)return 0;
    *entry=r.overlay.entry_vma;
    return 1;
}
int spmp_db_stage34_stage(void *ctx, const uint8_t *elf,
                         size_t n, uint32_t *entry) {
    spmp_db_stage34 *g=(spmp_db_stage34*)ctx;
    spmp_boot_result r;
    if(entry) *entry=0;
    if(!g || !g->layout || !g->platform || !elf || !entry)return 0;
    if(spmp_boot_stage(g->layout,g->platform,elf,n,&r)!=SPMP_BOOT_OK)return 0;
    if(!r.staged || !r.cache_confirmed || !r.overlay.executable ||
       !r.overlay.entry_vma)return 0;
    *entry=r.overlay.entry_vma;
    return 1;
}
