#include "spmp_memory.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>
static int add(size_t a,size_t b,size_t *out){
 if(a>SIZE_MAX-b)return -1;
 *out=a+b;return 0;
}
static int place(uint8_t *base,size_t cap,size_t *cursor,
 size_t requested,spmp_region *out){
 size_t pos=*cursor, aligned;
 uintptr_t address=(uintptr_t)base;
 if((uintptr_t)pos>UINTPTR_MAX-address)return -1;
 uintptr_t addr=address+pos;
 uintptr_t pad=(SPMP_MEMORY_ALIGNMENT-(addr&(SPMP_MEMORY_ALIGNMENT-1u)))&(SPMP_MEMORY_ALIGNMENT-1u);
 if(add(pos,(size_t)pad,&aligned)||aligned>cap||requested>cap-aligned)return -1;
 size_t end=aligned+requested;
 if(add(end,SPMP_MEMORY_GUARD_BYTES,cursor)||*cursor>cap)return -1;
 out->ptr=base+aligned;out->size=requested;
 memset(base+end,0xa5,SPMP_MEMORY_GUARD_BYTES);
 return 0;
}
int spmp_memory_partition(void *ram,size_t capacity,
 const spmp_partition_request *request,spmp_partition *result){
 if(!ram||!request||!result||!capacity)return -1;
 uint8_t *base=ram;
 /* Preflight compute using same alignment/guard formula, without writes. */
 size_t ptr=0;const size_t sizes[5]={request->system,request->core,request->game,request->video,request->audio};
 for(unsigned i=0;i<5;i++){
  uintptr_t at=(uintptr_t)base;
  if((uintptr_t)ptr>UINTPTR_MAX-at)return -1;
  uintptr_t pad=(SPMP_MEMORY_ALIGNMENT-((at+ptr)&(SPMP_MEMORY_ALIGNMENT-1u)))&(SPMP_MEMORY_ALIGNMENT-1u);
  if(add(ptr,(size_t)pad,&ptr)||add(ptr,sizes[i],&ptr)||
     add(ptr,SPMP_MEMORY_GUARD_BYTES,&ptr)||ptr>capacity)return -1;
 }
 spmp_partition tmp={0};ptr=0;
 if(place(base,capacity,&ptr,request->system,&tmp.system)||
    place(base,capacity,&ptr,request->core,&tmp.core)||
    place(base,capacity,&ptr,request->game,&tmp.game)||
    place(base,capacity,&ptr,request->video,&tmp.video)||
    place(base,capacity,&ptr,request->audio,&tmp.audio))return -1;
 tmp.used=ptr;tmp.unused=capacity-ptr;tmp.start=base;
 *result=tmp;return 0;
}
int spmp_memory_check_guards(const spmp_partition *part){
 if(!part||!part->start)return -1;
 const spmp_region *regions[5]={&part->system,&part->core,&part->game,&part->video,&part->audio};
 for(unsigned i=0;i<5;i++){
  uint8_t *guard=(uint8_t *)regions[i]->ptr+regions[i]->size;
  if(guard<part->start|| (uintptr_t)(guard-part->start)>part->used ||
     part->used-(size_t)(guard-part->start)<SPMP_MEMORY_GUARD_BYTES)return -1;
  for(unsigned k=0;k<SPMP_MEMORY_GUARD_BYTES;k++)if(guard[k]!=0xa5)return -1;
 }
 return 0;
}
void spmp_arena_init(spmp_arena *a,void *mem,size_t cap){
 if(!a)return;
 a->memory=mem;a->capacity=cap;a->used=0;a->peak=0;
}
void *spmp_arena_alloc(spmp_arena *a,size_t n,size_t align){
 if(!a||!a->memory||!align||(align&(align-1))||align>4096)return NULL;
 uintptr_t start=(uintptr_t)a->memory;
 if((uintptr_t)a->used>UINTPTR_MAX-start)return NULL;
 size_t pad=(size_t)((align-((start+a->used)&(align-1u)))&(align-1u));
 if(a->used>a->capacity||pad>a->capacity-a->used||n>a->capacity-a->used-pad)return NULL;
 size_t off=a->used+pad;
 a->used=off+n;if(a->used>a->peak)a->peak=a->used;
 return a->memory+off;
}
void spmp_arena_reset(spmp_arena *a){
 if(!a||!a->memory)return;
 if(a->used)memset(a->memory,0,a->used);
 a->used=0;
}
