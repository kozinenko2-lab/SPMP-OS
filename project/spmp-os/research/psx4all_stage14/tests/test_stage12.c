#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "spmp_psx_opt.h"
#include "spmp_psx_cache_guard.h"
#include "spmp_psx_budget.h"
#include "spmp_memory.h"
static int n;
#define CHECK(v) do { if (!(v)) {fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#v);exit(1);} ++n; } while(0)
static unsigned cleans,invalidates,drains,prefetches;
static uintptr_t minclean,maxclean,mininv,maxinv;
static void clean(uintptr_t a){if(!cleans) minclean=a; maxclean=a;cleans++;}
static void inval(uintptr_t a){if(!invalidates) mininv=a; maxinv=a;invalidates++;}
static void drain(void){drains++;}
static void prefetch(void){prefetches++;}
static spmp_psx_cache_ops ops={clean,drain,inval,prefetch};
static void test_cache(void){
  spmp_psx_cache_guard g={0};
  CHECK(!spmp_psx_icache_ready());
  CHECK(!spmp_psx_cache_guard_attach(&g,0x01000000,0x10000,0,&ops));
  CHECK(!spmp_psx_icache_ready());
  CHECK(!spmp_psx_cache_guard_attach(&g,0x01000001,0x10000,1,&ops));
  CHECK(!spmp_psx_cache_guard_attach(&g,0x01000000,0,1,&ops));
  CHECK(!spmp_psx_cache_guard_attach(&g,0x01000000,9u*1024u*1024u,1,&ops));
  CHECK(!spmp_psx_cache_guard_attach(&g,0x01000000,0x10000,1,NULL));
  CHECK(spmp_psx_cache_guard_attach(&g,0x01000000,0x10000,1,&ops));
  CHECK(spmp_psx_icache_ready());
  CHECK(spmp_psx_cache_guard_verified_range(0x01000000,0x10000));
  CHECK(!spmp_psx_cache_guard_verified_range(0x01000000,0x8000));
  spmp_psx_icache_sync(0x0100000d,0x01000031,0);
  CHECK(g.sync_calls==1 && !g.fault);
  CHECK(cleans==2 && invalidates==2 && drains==1 && prefetches==1);
  CHECK(minclean==0x01000000 && maxclean==0x01000020);
  CHECK(mininv==0x01000000 && maxinv==0x01000020);
  spmp_psx_icache_sync(0x00ffffff,0x01000004,0);
  CHECK(g.fault==1 && g.rejected==1 && g.sync_calls==1);
  CHECK(!spmp_psx_icache_ready());
  CHECK(!spmp_psx_cache_guard_verified_range(0x01000000,0x10000));
  spmp_psx_icache_sync(0x01000020,0x01000040,0);
  CHECK(cleans==2);
  spmp_psx_cache_guard_detach(&g);
  CHECK(!spmp_psx_icache_ready());
  CHECK(spmp_psx_cache_guard_faulted(&g));
  CHECK(!spmp_psx_arm926_privileged_ops()); /* no CP15 in host */
  CHECK(spmp_psx_cache_guard_attach(&g,0x01000000,0x10000,1,&ops));
  spmp_psx_icache_sync(0x01000020,0x01000020,0);
  CHECK(g.fault==1); /* empty range is invalid for generated code */
  spmp_psx_cache_guard_detach(&g);
}
static void test_bios_abs(void) {
 CHECK(spmp_psx_bios_abs32(0u)==0u);
 CHECK(spmp_psx_bios_abs32(123u)==123u);
 CHECK(spmp_psx_bios_abs32(0xffffffffu)==1u);
 CHECK(spmp_psx_bios_abs32(0x80000000u)==0x80000000u);
 CHECK(spmp_psx_bios_abs32(0x80000001u)==0x7fffffffu);
}
static void test_budget(void) {
  spmp_psx_budget b={0};
  CHECK(!spmp_psx_budget_plan(64u*1024u*1024u,512u,&b));
  CHECK(!spmp_psx_budget_plan(64u*1024u*1024u,9216u,&b));
  CHECK(spmp_psx_budget_plan(64u*1024u*1024u,4096u,&b));
  CHECK(b.jit_bytes==4096u*1024u && b.ram_bytes==2u*1024u*1024u);
  CHECK(!spmp_psx_budget_plan(b.total_bytes-1,4096u,&b));
  CHECK(spmp_psx_budget_plan(b.total_bytes,4096u,&b));
  CHECK(!spmp_psx_budget_plan(8u*1024u*1024u,8192u,&b));
  CHECK(spmp_psx_budget_plan(64u*1024u*1024u,1024u,&b));
  size_t small=b.total_bytes;
  CHECK(spmp_psx_budget_plan(64u*1024u*1024u,8192u,&b));
  CHECK(b.total_bytes>small);
  /* Test the actual Stage 10 partition manager with matching budgets. */
  const size_t capacity=b.total_bytes+8192u;
  uint8_t *ram=malloc(capacity);
  CHECK(ram!=NULL);
  spmp_partition_request req={0};spmp_partition parts={0};
  req.system=b.system_budget;
  req.core=b.jit_bytes+b.ram_bytes+b.bios_bytes+b.lut_bytes+b.core_code_budget;
  req.game=b.disc_cache_budget+b.misc_budget;
  req.video=b.vram_bytes+b.video_surface_budget;
  req.audio=b.spu_budget;
  CHECK(spmp_memory_partition(ram,capacity,&req,&parts)==0);
  CHECK(spmp_memory_check_guards(&parts)==0);
  CHECK(parts.core.size==req.core && parts.video.size==req.video);
  CHECK(spmp_memory_partition(ram,capacity/2,&req,&parts)!=0);
  CHECK(spmp_memory_check_guards(&parts)==0); /* failed probe cannot disturb guards */
  free(ram);
  printf("PSX4ALL Stage12: %d checks; 1/4/8 MiB JIT profiles; 8 MiB JIT budget=%zu bytes\n",n,b.total_bytes);
}
int main(void){test_cache();test_bios_abs();test_budget();return 0;}
