#include "spmp_stage44_handoff.h"
#include <stdint.h>
static uint32_t crc32(const uint8_t *p, size_t n) {
    uint32_t crc=~0u;
    for (size_t i=0;i<n;i++) {
        crc ^= p[i];
        for (int j=0;j<8;j++) crc=(crc>>1)^((crc&1u)?0xedb88320u:0u);
    }
    return ~crc;
}
static void copy_bytes(void *dest,const void *src,size_t n){
    uint8_t *d=(uint8_t *)dest;const uint8_t *s=(const uint8_t *)src;
    for(size_t i=0;i<n;i++)d[i]=s[i];
}
static int disjoint(const void *a,size_t an,const void *b,size_t bn) {
    uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
    if(an>UINTPTR_MAX-x || bn>UINTPTR_MAX-y)return 0;
    return x+an<=y || y+bn<=x;
}
int spmp_stage44_handoff_pack(void *region, size_t cap,
    const spmp_bootlog *log, uint32_t generation, int authorized) {
    spmp_stage44_handoff_header *h;
    uint8_t *payload;
    if(!authorized || !region || !log || cap<SPMP_HANDOFF_SIZE ||
       ((uintptr_t)region&3u) || !generation ||
       !disjoint(region,SPMP_HANDOFF_SIZE,log,sizeof(*log)) ||
       !spmp_log_is_valid(log)) return 0;
    h=(spmp_stage44_handoff_header *)region;
    h->commit=0;
    h->magic=SPMP_HANDOFF_MAGIC;
    h->version=SPMP_HANDOFF_VERSION;
    h->header_bytes=sizeof(*h);
    h->payload_bytes=sizeof(*log);
    h->session=log->session;
    h->generation=generation;
    payload=(uint8_t*)region+sizeof(*h);
    copy_bytes(payload,log,sizeof(*log));
    h->payload_crc32=crc32(payload,sizeof(*log));
    /* Compiler fence, NOT hardware cache maintenance or inter-core barrier. */
    __asm__ volatile ("" ::: "memory");
    h->commit=SPMP_HANDOFF_READY;
    return 1;
}
int spmp_stage44_handoff_unpack(const void *region, size_t cap,
    spmp_bootlog *dest, uint32_t expected_session,
    uint32_t expected_generation, int authorized) {
    const spmp_stage44_handoff_header *h;
    const uint8_t *payload;
    if(!authorized || !region || !dest || cap<SPMP_HANDOFF_SIZE ||
       ((uintptr_t)region&3u) || !expected_generation ||
       !disjoint(region,SPMP_HANDOFF_SIZE,dest,sizeof(*dest)))return 0;
    h=(const spmp_stage44_handoff_header *)region;
    if(h->magic!=SPMP_HANDOFF_MAGIC || h->version!=SPMP_HANDOFF_VERSION ||
       h->commit!=SPMP_HANDOFF_READY || h->header_bytes!=sizeof(*h) ||
       h->payload_bytes!=sizeof(*dest) || h->session!=expected_session ||
       h->generation!=expected_generation)return 0;
    payload=(const uint8_t*)region+sizeof(*h);
    if(crc32(payload,sizeof(*dest))!=h->payload_crc32)return 0;
    /* Validate payload before modifying caller-owned output. */
    if(!spmp_log_is_valid((const spmp_bootlog*)payload))return 0;
    copy_bytes(dest,payload,sizeof(*dest));
    return 1;
}
