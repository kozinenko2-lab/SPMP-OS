#include "native_binding.h"
struct binding_fingerprint { uint32_t slot,crc32; };
#ifdef SPMP_BINDING_SYNTHETIC
#include "binding_profile_synthetic.h"
#else
#include "binding_profile.h"
#endif
#define ENTRY 0x00a00000u
#define POOL_BEGIN 0x009fff00u
#define POOL_END 0x02000000u
extern unsigned char __runtime_end__[],__reserved_footer_end__[];
static uint32_t read32(uintptr_t address){return *(const volatile uint32_t *)address;}
static uint32_t prefix_crc32(uintptr_t address)
{
    const volatile uint8_t *p=(const volatile uint8_t *)address;
    uint32_t crc=0xffffffffu;
    for(uint32_t i=0;i<32;i++){
        crc^=p[i];
        for(uint32_t bit=0;bit<8;bit++)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));
    }
    return ~crc;
}
static int intersects(uint32_t a,uint32_t end,uint32_t b,uint32_t other_end)
{return a<other_end&&b<end;}
uint32_t spmp_native_preflight(uintptr_t raw_api)
{
    /* Raw pointer must match before dereferencing. Called in expected MMI BIN
     * context only; this is not a general readable-memory probe. No BSS writes.
     */
    if(raw_api!=0x005efb74u)return NATIVE_BAD_API;
    for(uint32_t i=0;i<85;i++)if(read32(raw_api+i*4)!=binding_slots[i])return NATIVE_BAD_SLOT;
    for(uint32_t i=0;i<9;i++){
        const struct binding_fingerprint *f=&binding_prefixes[i];
        if(prefix_crc32(binding_slots[f->slot])!=f->crc32)return NATIVE_BAD_CODE;
    }
    uint32_t base=read32(0x00693c5cu),capacity=read32(0x005641d8u);
    uint32_t runtime_end=(uint32_t)(uintptr_t)__runtime_end__;
    uint32_t file_bytes=(uint32_t)(uintptr_t)__reserved_footer_end__-ENTRY+128u;
    if(read32(0x005641dcu)!=6||base<POOL_BEGIN||base>ENTRY||(base&15u)||
       capacity>POOL_END-ENTRY||capacity<=file_bytes||runtime_end>ENTRY+capacity)
        return NATIVE_BAD_ALLOCATION;
    uint32_t allocation_end=ENTRY+capacity;
    uint32_t owner=read32(0x006769b0u),sp;
    __asm__ volatile("mov %0, sp":"=r"(sp));
    if(owner<POOL_BEGIN||(owner&15u)||owner>POOL_END-0x100010u||
       intersects(base,allocation_end,owner,owner+0x100010u)||
       sp<owner+0x2000u||sp>owner+0x100000u)return NATIVE_BAD_STACK;
    uint32_t frames[2]={read32(0x005641e0u),read32(0x005641e4u)};
    for(uint32_t i=0;i<2;i++){
        uint32_t frame=frames[i];
        if(frame<POOL_BEGIN||(frame&15u)||frame>POOL_END-0x25800u||
           intersects(base,allocation_end,frame,frame+0x25800u)||
           intersects(owner,owner+0x100010u,frame,frame+0x25800u))return NATIVE_BAD_FRAMEBUFFERS;
    }
    if(intersects(frames[0],frames[0]+0x25800u,frames[1],frames[1]+0x25800u))return NATIVE_BAD_FRAMEBUFFERS;
    return NATIVE_BOUND;
}
uint32_t spmp_native_bind(uintptr_t raw_api,struct probe_oem_api *out)
{
    volatile struct probe_oem_api *cleared=out;
    cleared->init_graph=0;
    cleared->set_display_origin=0;
    cleared->enable_double_buffer=0;
    cleared->dimensions=0;
    cleared->get_color=0;
    cleared->set_color=0;
    cleared->fill=0;
    cleared->paint=0;
    cleared->keys=0;
    uint32_t status=spmp_native_preflight(raw_api);
    if(status)return status;
    /* All checks complete before publishing any callable pointer. */
    out->init_graph=(mmi_init_graph_fn)(uintptr_t)binding_slots[0x38/4];
    out->set_display_origin=(mmi_set_display_origin_fn)(uintptr_t)binding_slots[0x54/4];
    out->enable_double_buffer=(mmi_enable_double_buffer_fn)(uintptr_t)binding_slots[0xa8/4];
    out->dimensions=(mmi_get_dimensions_fn)(uintptr_t)binding_slots[0x94/4];
    out->get_color=(mmi_get_fg_fn)(uintptr_t)binding_slots[0x48/4];
    out->set_color=(mmi_set_fg_fn)(uintptr_t)binding_slots[0x44/4];
    out->fill=(mmi_fill_rect_fn)(uintptr_t)binding_slots[0xc4/4];
    out->paint=(mmi_paint_fn)(uintptr_t)binding_slots[0x10/4];
    out->keys=(mmi_get_keys_fn)(uintptr_t)binding_slots[0x100/4];
    return NATIVE_BOUND;
}
