#include "spmp_boot_gate.h"
#include <string.h>
#include <stdint.h>

static int good(spmp_boot_range r){
    return r.bytes && (uint64_t)r.start + r.bytes <= UINT32_MAX;
}
static int contained(spmp_boot_range total, spmp_boot_range r){
    return good(total) && good(r) && r.start >= total.start &&
           (uint64_t)r.start+r.bytes <= (uint64_t)total.start+total.bytes;
}
static int overlap(spmp_boot_range a,spmp_boot_range b){
    return (uint64_t)a.start < (uint64_t)b.start+b.bytes &&
           (uint64_t)b.start < (uint64_t)a.start+a.bytes;
}
static int ptr_overlap(const uint8_t *a,size_t n, const uint8_t *b,size_t m){
    uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
    /* Overflow is invalid, NOT proof of disjointness. */
    if(n>UINTPTR_MAX-x||m>UINTPTR_MAX-y)return -1;
    return x<y+m && y<x+n;
}
int spmp_boot_validate_layout(const spmp_boot_layout *l){
    if(!l || !l->board_revision || !contained(l->ram,l->module_slot) ||
       !l->reserved_count || l->reserved_count>SPMP_BOOT_MAX_RESERVED ||
       (l->module_slot.start&4095u) ||
       (l->module_slot.bytes&3u) ||
       l->module_slot.bytes>SPMP_OVERLAY_MAX_BYTES)return SPMP_BOOT_BAD_MAP;
    for(unsigned i=0;i<l->reserved_count;i++){
        if(!contained(l->ram,l->reserved[i]) ||
           overlap(l->reserved[i],l->module_slot))return SPMP_BOOT_BAD_MAP;
        for(unsigned j=0;j<i;j++)
            if(overlap(l->reserved[i],l->reserved[j]))return SPMP_BOOT_BAD_MAP;
    }
    return SPMP_BOOT_OK;
}
int spmp_boot_preflight(const spmp_boot_layout *l,const uint8_t *elf,
    size_t n,spmp_boot_result *result){
    if(!result)return SPMP_BOOT_BAD_INPUT;
    memset(result,0,sizeof *result); /* no stale readiness on failure */
    if(!elf || !n)return SPMP_BOOT_BAD_INPUT;
    int rc=spmp_boot_validate_layout(l);
    if(rc)return rc;
    spmp_overlay_options opt={0};opt.virtual_base=l->module_slot.start;
    spmp_overlay_image im;
    rc=spmp_overlay_probe(elf,n,l->module_slot.bytes,&opt,&im);
    if(rc)return SPMP_BOOT_OVERLAY_ERROR;
    /* The trampoline is not installed here: entry is only an address for the
     * platform team to examine after device-specific ABI verification. */
    if(im.used_bytes>l->module_slot.bytes ||
       (uint64_t)im.entry_vma < l->module_slot.start ||
       (uint64_t)im.entry_vma >= (uint64_t)l->module_slot.start+im.used_bytes)
        return SPMP_BOOT_OVERLAY_ERROR;
    spmp_boot_result tmp={0};tmp.slot=l->module_slot;tmp.overlay=im;
    *result=tmp;
    return SPMP_BOOT_OK;
}
int spmp_boot_stage(const spmp_boot_layout *l,const spmp_boot_platform *ops,
    const uint8_t *elf,size_t n,spmp_boot_result *result){
    if(!result)return SPMP_BOOT_BAD_INPUT;
    memset(result,0,sizeof *result); /* fail-closed if previous stage passed */
    if(!ops||!ops->confirm_slot_owned||!ops->map_slot||!ops->clean_dcache||
       !ops->invalidate_icache)return SPMP_BOOT_BAD_INPUT;
    spmp_boot_result preflight;
    int rc=spmp_boot_preflight(l,elf,n,&preflight);
    if(rc)return rc;
    /* Output must be cleared on ANY failure. No previous buffer is touched by
     * invalid input. Valid input may modify a slot after ownership approval. */
    if(ops->confirm_slot_owned(ops->context,l->module_slot.start,
                              l->module_slot.bytes)!=1)return SPMP_BOOT_NOT_ATTESTED;
    uint8_t *dst=ops->map_slot(ops->context,l->module_slot.start,
                               l->module_slot.bytes);
    if(!dst)return SPMP_BOOT_UNMAPPED;
    if(ptr_overlap(elf,n,dst,l->module_slot.bytes)!=0)return SPMP_BOOT_UNSAFE_ALIAS;
    spmp_overlay_options opt={0};opt.virtual_base=l->module_slot.start;
    spmp_overlay_image im;
    rc=spmp_overlay_load(elf,n,dst,l->module_slot.bytes,&opt,&im);
    if(rc)return SPMP_BOOT_OVERLAY_ERROR;
    /* spmp_overlay_load deliberately leaves .executable=0 without hooks.
     * This wrapper checks independently that BOTH cache operations succeeded. */
    if(ops->clean_dcache(ops->context,dst,im.used_bytes)!=1 ||
       ops->invalidate_icache(ops->context,dst,im.used_bytes)!=1){
        memset(dst,0,im.used_bytes); /* fail closed: no execution permission */
        (void)ops->clean_dcache(ops->context,dst,im.used_bytes);
        return SPMP_BOOT_CACHE_FAILED;
    }
    preflight.overlay=im;
    preflight.overlay.executable=1; /* cache-ready, not a userland process */
    preflight.staged=1;preflight.cache_confirmed=1;
    *result=preflight;
    return SPMP_BOOT_OK;
}
const char *spmp_boot_status_text(int code){
    switch(code){
        case SPMP_BOOT_OK:return "dry-run/staging successful";
        case SPMP_BOOT_BAD_INPUT:return "incomplete API parameters";
        case SPMP_BOOT_BAD_MAP:return "invalid or overlapping memory map";
        case SPMP_BOOT_NOT_ATTESTED:return "platform did not confirm RAM ownership";
        case SPMP_BOOT_UNMAPPED:return "platform did not map writable RAM";
        case SPMP_BOOT_UNSAFE_ALIAS:return "ELF overlaps target slot or overflows pointer";
        case SPMP_BOOT_CACHE_FAILED:return "cache maintenance not confirmed";
        case SPMP_BOOT_OVERLAY_ERROR:return "ELF overlay preflight/load rejected";
        default:return "unknown error";
    }
}
