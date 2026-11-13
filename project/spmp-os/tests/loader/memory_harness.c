#include "spmp_memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
int main(void){
 unsigned char storage[1024*1024+128];
 memset(storage,0x44,sizeof storage);
 spmp_partition_request spec={
 .system=128*1024,.core=384*1024,.game=256*1024,
 .video=128*1024,.audio=32*1024};
 spmp_partition part;
 assert(spmp_memory_partition(storage,sizeof storage,&spec,&part)==0);
 assert(spmp_memory_check_guards(&part)==0);
 assert(part.used<=sizeof storage && part.unused>0);
 assert(part.core.size==spec.core && part.game.size==spec.game);
 assert((uintptr_t)part.core.ptr%64==0 && (uintptr_t)part.game.ptr%64==0);
 spmp_arena game;spmp_arena_init(&game,part.game.ptr,part.game.size);
 for(int j=0;j<1000;j++){
  unsigned char *rom=spmp_arena_alloc(&game,64*1024,16);
  uint32_t *ram=spmp_arena_alloc(&game,128*1024,64);
  assert(rom && ram);
  memset(rom,j&255,64*1024);
  ram[0]=0x12345678;
  assert(!spmp_arena_alloc(&game,2*1024*1024,4));
  assert(spmp_memory_check_guards(&part)==0);
  spmp_arena_reset(&game);
  assert(game.used==0 && rom[0]==0 && ram[0]==0);
  assert(spmp_memory_check_guards(&part)==0);
 }
 assert(game.peak>=192*1024 && game.peak<=193*1024);
 /* Oversized plan must be rejected before touching any memory. */
 spmp_partition_request bad=spec;bad.core=2*1024*1024;
 assert(spmp_memory_partition(storage,sizeof storage,&bad,&part)!=0);
 assert(spmp_memory_check_guards(&part)==0);
 ((uint8_t *)part.video.ptr)[part.video.size]=0xff;
 assert(spmp_memory_check_guards(&part)!=0); /* detected guard overwrite */
 puts("Stage10 memory: 5 guarded regions; 1000 ROM replacements; high-water verified; invalid plans rejected");
 return 0;
}
