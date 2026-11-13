#include "spmp_boot_gate.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
typedef struct {unsigned confirms,maps,cleans,invalidates;int trust,cache_ok,force_alias;uint8_t slot[16384+32];} simulated;
static int confirm(void *ctx,uint32_t start,uint32_t size){
    simulated *s=ctx;s->confirms++;assert(start==0x40004000u && size==16384u);return s->trust;
}
static void *map(void *ctx,uint32_t start,uint32_t size){
    simulated *s=ctx;s->maps++;assert(start==0x40004000u && size==16384u);
    return s->slot+16;
}
static int clean(void *ctx,void *dst,size_t n){
    simulated *s=ctx;assert(dst==s->slot+16 && n>0 && n<16384);s->cleans++;return s->cache_ok;
}
static int invalidate(void *ctx,void *dst,size_t n){
    simulated *s=ctx;assert(dst==s->slot+16 && n>0 && n<16384);s->invalidates++;return 1;
}
static uint8_t *readfile(const char *name,size_t *length){
    FILE *f=fopen(name,"rb");assert(f);assert(!fseek(f,0,SEEK_END));
    long n=ftell(f);assert(n>0 && n<1024*1024);rewind(f);
    uint8_t *data=malloc((size_t)n);assert(data);
    assert(fread(data,1,(size_t)n,f)==(size_t)n);fclose(f);*length=(size_t)n;return data;
}
static spmp_boot_layout layout(void){
    spmp_boot_layout l={0};
    /* Test-only addresses, NOT a RAM map for any physical Defender model. */
    l.board_revision=0x1234u;
    l.ram=(spmp_boot_range){0x40000000u,0x200000u};
    l.module_slot=(spmp_boot_range){0x40004000u,16384u};
    l.reserved_count=2;
    l.reserved[0]=(spmp_boot_range){0x40000000u,8192u};
    l.reserved[1]=(spmp_boot_range){0x40010000u,32768u};
    return l;
}
static void sentinels(const simulated *s){
    for(size_t i=0;i<16;i++)assert(s->slot[i]==0x51&&s->slot[16400+i]==0xa7);
}
int main(int argc,char **argv){
    assert(argc==2);
    size_t n;uint8_t *elf=readfile(argv[1],&n);
    simulated *s=calloc(1,sizeof(*s));assert(s);
    memset(s->slot,0x51,16);memset(s->slot+16,0xcc,16384);
    memset(s->slot+16400,0xa7,16);
    spmp_boot_layout l=layout();spmp_boot_result r={0};
    spmp_boot_platform platform={s,confirm,map,clean,invalidate};
    assert(spmp_boot_validate_layout(&l)==0);
    assert(spmp_boot_preflight(&l,elf,n,&r)==0);
    assert(r.overlay.entry_vma>=l.module_slot.start && r.overlay.used_bytes<16384);
    assert(r.staged==0 && r.cache_confirmed==0);
    assert(s->confirms==0 && s->maps==0);
    assert(spmp_boot_stage(&l,&platform,elf,n,&r)==SPMP_BOOT_NOT_ATTESTED);
    assert(s->maps==0 && s->cleans==0 && s->invalidates==0);
    s->trust=1;s->cache_ok=1;
    assert(spmp_boot_stage(&l,&platform,elf,n,&r)==0);
    assert(r.staged && r.cache_confirmed && r.overlay.executable);
    assert(s->confirms==2 && s->maps==1 && s->cleans==1 && s->invalidates==1);
    sentinels(s);
    /* Proof of invalid ELF failure before even querying memory ownership. */
    uint8_t *bad=malloc(n);assert(bad);memcpy(bad,elf,n);bad[18]=0;
    unsigned confirms=s->confirms,maps=s->maps;
    assert(spmp_boot_stage(&l,&platform,bad,n,&r)==SPMP_BOOT_OVERLAY_ERROR);
    assert(s->confirms==confirms && s->maps==maps);
    /* Guard a dozen individual layout rejection cases. */
    for(unsigned i=0;i<12;i++){
      spmp_boot_layout x=l;
      switch(i){
       case 0:x.board_revision=0;break;
       case 1:x.reserved_count=0;break;
       case 2:x.reserved_count=13;break;
       case 3:x.module_slot.start=0x40003ffcu;break;
       case 4:x.module_slot.start=0x40000000u;break;
       case 5:x.module_slot.start=0x40010000u;break;
       case 6:x.module_slot.start=0x400ff000u;x.module_slot.bytes=0x200000u;break;
       case 7:x.module_slot.bytes=0;break;
       case 8:x.module_slot.bytes=SPMP_OVERLAY_MAX_BYTES+4u;break;
       case 9:x.reserved[1].start=x.reserved[0].start;break;
       case 10:x.ram.bytes=1024;break;
       case 11:x.reserved[0].bytes=0;break;
      }
      assert(spmp_boot_validate_layout(&x)==SPMP_BOOT_BAD_MAP);
    }
    /* Cache failure wipes bytes and never returns ready-to-dispatch. */
    size_t used=r.overlay.used_bytes;
    s->cache_ok=0;
    assert(spmp_boot_stage(&l,&platform,elf,n,&r)==SPMP_BOOT_CACHE_FAILED);
    assert(!r.staged && !r.cache_confirmed && !r.overlay.executable);
    for(size_t i=0;i<used;i++)assert(s->slot[16+i]==0);
    sentinels(s);
    /* A valid ELF in the mapped destination is a true aliased input.
       Preflight succeeds but the write is vetoed before any relocation. */
    assert(n<16384u);
    memcpy(s->slot+16,elf,n);
    uint8_t *snapshot=malloc(n);assert(snapshot);
    memcpy(snapshot,s->slot+16,n);
    unsigned old_clean=s->cleans;
    assert(spmp_boot_stage(&l,&platform,s->slot+16,n,&r)==SPMP_BOOT_UNSAFE_ALIAS);
    assert(s->cleans==old_clean && !r.staged);
    assert(!memcmp(snapshot,s->slot+16,n));
    free(snapshot);
    sentinels(s);
    /* Fuzz layout arithmetic; no callbacks and no memory writes on failure. */
    uint32_t rng=0x7edff321u;unsigned rejected=0;
    for(unsigned i=0;i<20000;i++){
       rng=rng*1664525u+1013904223u;
       spmp_boot_layout x=l;x.module_slot.start=(rng & 0xfffff000u);
       rng=rng*1664525u+1013904223u;
       x.module_slot.bytes=(rng&0xffffu)+1u;
       int valid=spmp_boot_validate_layout(&x);
       if(valid==SPMP_BOOT_BAD_MAP)rejected++;
       else assert(valid==SPMP_BOOT_OK);
    }
    assert(rejected>19000);
    printf("Stage34 boot gate: dry-run, callback attestation, 12 bad maps, 20000 fuzz layouts, cache fail-close PASS\n");
    free(bad);free(elf);free(s);return 0;
}
