#include "spmp_psx_budget.h"
#include <stdint.h>
#define K 1024u
static int add(size_t *v,size_t x){if(*v>SIZE_MAX-x)return 0;*v+=x;return 1;}
int spmp_psx_budget_plan(size_t cap,unsigned jit_kb,spmp_psx_budget *out) {
    spmp_psx_budget b={0}; size_t sum=0;
    if(!out || (jit_kb!=1024u && jit_kb!=2048u && jit_kb!=4096u && jit_kb!=8192u)) return 0;
    b.jit_bytes=(size_t)jit_kb*K; b.ram_bytes=2048u*K;
    b.bios_bytes=512u*K;b.lut_bytes=256u*K;
    b.vram_bytes=1024u*512u*2u;
    b.spu_budget=512u*K;
    b.core_code_budget=1536u*K; /* measured after complete ARM build */
    b.system_budget=1024u*K;
    b.disc_cache_budget=256u*K;
    b.video_surface_budget=320u*240u*2u*2u;
    b.misc_budget=512u*K; /* heap, stacks, guards, GPIO/FS staging */
    if(!add(&sum,b.jit_bytes)||!add(&sum,b.ram_bytes)||
       !add(&sum,b.bios_bytes)||!add(&sum,b.lut_bytes)||
       !add(&sum,b.vram_bytes)||!add(&sum,b.spu_budget)||
       !add(&sum,b.core_code_budget)||!add(&sum,b.system_budget)||
       !add(&sum,b.disc_cache_budget)||!add(&sum,b.video_surface_budget)||
       !add(&sum,b.misc_budget)) return 0;
    b.total_bytes=sum;
    *out=b;
    return sum<=cap;
}
