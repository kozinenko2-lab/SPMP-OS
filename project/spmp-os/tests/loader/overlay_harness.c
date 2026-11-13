/* Run with two independently compiled ARMv5 ELF32 objects. Host tests do not
 * execute ARM instructions; only inspect, relocate and allocate them. */
#include "spmp_overlay.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int dcache_calls,icache_calls;
static void dcache(void *ctx,void *ptr,size_t n){(void)ctx;assert(ptr && n);dcache_calls++;}
static void icache(void *ctx,void *ptr,size_t n){(void)ctx;assert(ptr && n);icache_calls++;}
static uint8_t *load_file(const char *file,size_t *size){
 FILE *f=fopen(file,"rb");assert(f);assert(!fseek(f,0,SEEK_END));
 long len=ftell(f);assert(len>0 && len<1024*1024);rewind(f);
 uint8_t *b=malloc((size_t)len);assert(b);
 assert(fread(b,1,(size_t)len,f)==(size_t)len);fclose(f);*size=(size_t)len;return b;
}
static uint32_t r32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void w32(uint8_t *p,uint32_t v){for(unsigned j=0;j<4;j++)p[j]=(uint8_t)(v>>(j*8));}
static uint16_t r16(const uint8_t *p){return p[0]|(uint16_t)p[1]<<8;}
static void check_canary(const uint8_t *memory,size_t capacity){
 for(size_t i=0;i<16;i++)assert(memory[i]==0x5a && memory[16+capacity+i]==0xa5);
}
int main(int argc,char **argv){
 if(argc!=3){fprintf(stderr,"usage: %s probe.o alt.o\n",argv[0]);return 2;}
 size_t na,nb;uint8_t *a=load_file(argv[1],&na),*b=load_file(argv[2],&nb);
 enum { CAP=8192 }; /* fake, bounded RAM; nothing maps at 0x20000000 */
 uint8_t region[CAP+32];memset(region,0x5a,16);
 memset(region+16,0xcc,CAP);memset(region+16+CAP,0xa5,16);
 spmp_overlay_options opt={.virtual_base=0x20000000u};
 spmp_overlay_manager mgr;spmp_overlay_manager_init(&mgr,region+16,CAP,&opt);
 spmp_overlay_image m={0};int rc;
 rc=spmp_overlay_probe(a,na,CAP,&opt,&m);
 assert(rc==0 && m.entry_vma>=opt.virtual_base && m.used_bytes<CAP);
 assert(m.relocation_count>=2 && !m.executable);
 assert(strcmp(m.meta.core_id,"stage10-probe")==0);
 /* Small capacity cannot fit the module, but leaves guard bytes unchanged. */
 assert(spmp_overlay_probe(a,na,8,&opt,&m)==SPMP_OV_NO_MEMORY);
 assert(!memcmp(region+16,"\xcc\xcc\xcc\xcc",4));
 /* No cache sync => the loader must not claim ARM execution readiness. */
 rc=spmp_overlay_manager_switch(&mgr,a,na,&m);
 assert(rc==0 && mgr.loaded && !m.executable && mgr.generations==1);
 assert(strcmp(m.meta.core_id,"stage10-probe")==0);
 size_t used=m.used_bytes;
 uint8_t snapshot[CAP];memcpy(snapshot,region+16,CAP);
 rc=spmp_overlay_manager_switch(&mgr,a,na,&m);
 assert(rc==1 && mgr.generations==1 && !memcmp(snapshot,region+16,CAP));
 /* Rebuilt core with identical ID but different payload must reload. */
 uint8_t *changed=malloc(na+1);assert(changed);
 memcpy(changed,a,na);changed[na]=0x5cu;
 assert(spmp_overlay_manager_switch(&mgr,changed,na+1,&m)==0);
 assert(mgr.generations==2 && !memcmp(snapshot,region+16,CAP));
 free(changed);
 /* Malformed ELF cannot dislodge the existing resident core. */
 uint8_t *bad=malloc(na);assert(bad);memcpy(bad,a,na);
 bad[18]=0x3e;assert(spmp_overlay_manager_switch(&mgr,bad,na,&m)==SPMP_OV_BAD_ELF);
 assert(mgr.generations==2 && !memcmp(snapshot,region+16,CAP));
 memcpy(bad,a,na);bad[48]=0xff;bad[49]=0x7f;
 assert(spmp_overlay_manager_switch(&mgr,bad,na,&m)==SPMP_OV_BAD_ELF);
 assert(mgr.generations==2 && !memcmp(snapshot,region+16,CAP));
 memcpy(bad,a,na);w32(bad+32,0xfffffff0u);
 assert(spmp_overlay_manager_switch(&mgr,bad,na,&m)==SPMP_OV_BAD_ELF);
 assert(mgr.generations==2 && !memcmp(snapshot,region+16,CAP));
 /* All relocation types are checked BEFORE changing live RAM. */
 memcpy(bad,a,na);
 uint16_t sections=r16(bad+48);size_t sht=r32(bad+32);int relocation_changed=0;
 for(unsigned i=0;i<sections;i++){
  size_t q=sht+i*40;if(r32(bad+q+4)==9 && r32(bad+q+20)>=8){
   size_t rel=r32(bad+q+16);
   w32(bad+rel+4,(r32(bad+rel+4)&0xffffff00u)|0xfeu);relocation_changed=1;break;
  }
 }
 assert(relocation_changed);
 assert(spmp_overlay_manager_switch(&mgr,bad,na,&m)==SPMP_OV_UNSUPPORTED);
 assert(mgr.generations==2 && !memcmp(snapshot,region+16,CAP));
 /* An older host callback ABI is NOT interchangeable: reject before
  * swapping the resident executable or its RAM. */
 memcpy(bad,a,na);
 int found_meta=0;
 for(size_t i=0;i+52<=na;i++){
  if(!memcmp(bad+i,"SPMP",4)&&r32(bad+i+4)==1&&r32(bad+i+8)==5){
   w32(bad+i+8,4);found_meta=1;break;
  }
 }
 assert(found_meta);
 assert(spmp_overlay_manager_switch(&mgr,bad,na,&m)==SPMP_OV_BAD_META);
 assert(mgr.generations==2 && !memcmp(snapshot,region+16,CAP));
 /* Valid alt core triggers unload+load, even after invalid module attempts. */
 opt.dcache_clean=dcache;opt.icache_invalidate=icache;
 mgr.options=opt;
 assert(spmp_overlay_manager_switch(&mgr,b,nb,&m)==0);
 assert(m.executable && mgr.generations==3 && dcache_calls==1 && icache_calls==1);
 assert(strcmp(m.meta.core_id,"stage10-alt")==0);
 assert(spmp_overlay_manager_switch(&mgr,b,nb,&m)==1 && mgr.generations==3);
 assert(spmp_overlay_manager_switch(&mgr,a,na,&m)==0 && mgr.generations==4);
 assert(m.executable && dcache_calls==2 && icache_calls==2);
 assert(strcmp(m.meta.core_id,"stage10-probe")==0);
 assert(m.used_bytes==used);
 spmp_overlay_manager_unload(&mgr);assert(!mgr.loaded);
 for(size_t i=0;i<used;i++)assert(region[16+i]==0);
 check_canary(region,CAP);
 /* Unsafe config or incomplete metadata must never be accepted. */
 opt.virtual_base=0x20000001u;
 assert(spmp_overlay_probe(a,na,CAP,&opt,&m)==SPMP_OV_BAD_INPUT);
 free(a);free(b);free(bad);
 printf("Stage10 loader: ARMv5 ELF32 + %u relocated modules, checks=%d, guards=OK, cache hooks=OK, 4 switch generations OK\n",2,15);
 return 0;
}
