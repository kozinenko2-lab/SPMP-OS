#include "spmp_psx_jit_store.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

static size_t live_bytes;
static unsigned allocations, releases;
static int deny_next;
static void *test_alloc(size_t n, void *opaque) {
    (void)opaque;
    if (deny_next) { deny_next=0; return NULL; }
    void *p = malloc(n);
    if (p) { ++allocations; live_bytes += n; }
    return p;
}
static void test_free(void *p, void *opaque) {
    (void)opaque;
    ++releases;
    /* length tested before calling release; do not use freed struct */
    free(p);
}
static void test_one(size_t bytes) {
    spmp_psx_jit_store s = {0};
    assert(spmp_psx_jit_store_init(&s,bytes,test_alloc,NULL));
    assert(s.code && s.ram && s.rom && s.lut);
    assert(s.code_capacity == bytes);
    assert(((uintptr_t)s.code & 63u) == 0u);
    assert((uint8_t *)s.ram == s.code + bytes);
    assert((uint8_t *)s.rom == (uint8_t *)s.ram + SPMP_PSX_JIT_RAM_BYTES);
    assert((uint8_t *)s.lut == (uint8_t *)s.rom + SPMP_PSX_JIT_ROM_BYTES);
    assert(s.code[0]==0 && s.code[bytes-1]==0);
    assert(s.ram[SPMP_PSX_JIT_RAM_BYTES-1]==0);
    assert(s.rom[SPMP_PSX_JIT_ROM_BYTES-1]==0);
    assert(s.lut[65535]==0);
    assert(!spmp_psx_jit_store_init(&s,bytes,test_alloc,NULL)); /* no double init */
    s.code[0] = 0xea;
    s.code[bytes-1] = 0xff;
    s.ram[SPMP_PSX_JIT_RAM_BYTES-1] = 65;
    s.rom[SPMP_PSX_JIT_ROM_BYTES-1] = 73;
    s.lut[65535] = 0x12345678u;
    size_t length = s.bytes_owned;
    spmp_psx_jit_store_release(&s,test_free,NULL);
    live_bytes -= length;
    assert(!s.allocation && !s.code && !s.ram && !s.rom && !s.lut);
    assert(!s.bytes_owned && !s.code_capacity);
    spmp_psx_jit_store_release(&s,test_free,NULL); /* idempotent */
}
int main(void) {
    spmp_psx_jit_store s={0};
    assert(!spmp_psx_jit_store_init(NULL,1024u*1024u,test_alloc,NULL));
    assert(!spmp_psx_jit_store_init(&s,0,test_alloc,NULL));
    assert(!spmp_psx_jit_store_init(&s,1023u*1024u,test_alloc,NULL));
    assert(!spmp_psx_jit_store_init(&s,8193u*1024u,test_alloc,NULL));
    assert(!spmp_psx_jit_store_init(&s,1024u*1024u+1,test_alloc,NULL));
    assert(!spmp_psx_jit_store_init(&s,1024u*1024u,NULL,NULL));
    deny_next=1;
    assert(!spmp_psx_jit_store_init(&s,1024u*1024u,test_alloc,NULL));
    assert(!s.allocation && !s.code && !s.bytes_owned);
    const size_t sizes[]={1024u*1024u,2048u*1024u,4096u*1024u,8192u*1024u};
    for (unsigned cycle=0;cycle<250;cycle++) test_one(sizes[cycle%4]);
    assert(live_bytes==0 && allocations==releases);
    printf("Stage13 JIT arena: 250 lifecycle cycles, 4 capacity profiles, "
           "invalid sizes/OOM/reinit/release PASS (allocs=%u frees=%u)\n",
           allocations,releases);
    return 0;
}
