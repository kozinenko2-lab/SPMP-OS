/* Strict subset of ARM EABI relocatable ELF32 loader for SPMP-OS.
 * The unit tests exercise real Clang-produced arm926ej-s object files.
 * No implicit imports; no Thumb; no relocation types beyond audited subset.
 */
#include "spmp_overlay.h"
#include "spmp_cores.h"
#include <string.h>
#include <limits.h>

#define SHT_NULL 0u
#define SHT_PROGBITS 1u
#define SHT_SYMTAB 2u
#define SHT_STRTAB 3u
#define SHT_RELA 4u
#define SHT_NOBITS 8u
#define SHT_REL 9u
#define SHF_ALLOC 2u
#define SHF_EXEC 4u
#define SHN_UNDEF 0u
#define SHN_ABS 0xfff1u
#define ARM_ABS32 2u
#define ARM_REL32 3u
#define ARM_PC24 1u
#define ARM_CALL 28u
#define ARM_JUMP24 29u
#define ARM_PREL31 42u

typedef struct {uint32_t type, flags, off, size, link, info, align, entsize; uint32_t addr;} shdr;
typedef struct {
 const uint8_t *elf;
 size_t length, cap;
 uint32_t base;
 uint16_t num, shstr;
 uint32_t shtoff, offs[SPMP_OVERLAY_MAX_SECTIONS];
 uint8_t allocated[SPMP_OVERLAY_MAX_SECTIONS];
 size_t total;
 uint32_t relocs;
 spmp_overlay_image image;
} plan;

static uint16_t r16(const uint8_t *p){return (uint16_t)((uint16_t)p[0]|(uint16_t)p[1]<<8);}
static uint32_t r32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void w32(uint8_t *p,uint32_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);p[2]=(uint8_t)(v>>16);p[3]=(uint8_t)(v>>24);}
static int bounds(size_t total, uint64_t off,uint64_t n){return off<=total&&n<=(uint64_t)total-off;}
static int terminated(const uint8_t *s,size_t n){return memchr(s,0,n)!=NULL;}
static int name_eq(const uint8_t *table,size_t size,uint32_t off,const char *name){
 size_t n=strlen(name);return off<size && n<size-off && !memcmp(table+off,name,n) && !table[off+n];
}
static int section(const plan *p,uint16_t i,shdr *s){
 if(i>=p->num)return SPMP_OV_BAD_ELF;
 const uint8_t *q=p->elf+p->shtoff+(size_t)i*40;
 s->type=r32(q+4);s->flags=r32(q+8);s->addr=r32(q+12);
 s->off=r32(q+16);s->size=r32(q+20);s->link=r32(q+24);
 s->info=r32(q+28);s->align=r32(q+32);s->entsize=r32(q+36);
 return SPMP_OV_OK;
}
static int read_symbol(const plan *p,const shdr *syms,uint32_t index,
 uint32_t *sym_vma,uint32_t *name_off,const shdr *strs){
 if(syms->entsize!=16||index>=syms->size/16)return SPMP_OV_BAD_SYMBOL;
 if(!bounds(p->length,syms->off+(uint64_t)index*16,16))return SPMP_OV_BAD_SYMBOL;
 const uint8_t *q=p->elf+syms->off+(size_t)index*16;
 uint32_t off=r32(q+4);uint16_t index_section=r16(q+14);
 *name_off=r32(q);
 if(*name_off>=strs->size || !terminated(p->elf+strs->off+*name_off,strs->size-*name_off))return SPMP_OV_BAD_SYMBOL;
 if(index_section==SHN_UNDEF||index_section==SHN_ABS||index_section>=p->num||!p->allocated[index_section])return SPMP_OV_BAD_SYMBOL;
 shdr def;section(p,index_section,&def);
 if(off>def.size)return SPMP_OV_BAD_SYMBOL;
 if((uint64_t)p->base+p->offs[index_section]+off>UINT32_MAX)return SPMP_OV_OVERFLOW;
 *sym_vma=p->base+p->offs[index_section]+off;return SPMP_OV_OK;
}
static int symtab_for(const plan *p,const shdr *s,shdr *strs){
 if(s->link>=p->num||s->type!=SHT_SYMTAB||s->entsize!=16||s->size%16)return SPMP_OV_BAD_ELF;
 section(p,(uint16_t)s->link,strs);
 if(strs->type!=SHT_STRTAB||!bounds(p->length,strs->off,strs->size)||
    !bounds(p->length,s->off,s->size))return SPMP_OV_BAD_ELF;
 return 0;
}
static int relocation(const plan *p,const shdr *t,const uint8_t *item,
 uint32_t *patched){
 uint32_t off=r32(item), info=r32(item+4);
 uint32_t type=info&255u, symbol=info>>8;
 shdr target;section(p,(uint16_t)t->info,&target);
 if(target.type==SHT_NOBITS || off>target.size || target.size-off<4u)return SPMP_OV_BAD_RELOCATION;
 if(!bounds(p->length,(uint64_t)target.off+off,4))return SPMP_OV_BAD_RELOCATION;
 shdr syms,strs;section(p,(uint16_t)t->link,&syms);
 if(symtab_for(p,&syms,&strs))return SPMP_OV_BAD_ELF;
 uint32_t s,n;int ret=read_symbol(p,&syms,symbol,&s,&n,&strs);if(ret)return ret;
 (void)n;
 uint32_t orig=r32(p->elf+target.off+off);
 uint64_t place=(uint64_t)p->base+p->offs[t->info]+off;
 if(place>UINT32_MAX)return SPMP_OV_OVERFLOW;
 int64_t a=(int32_t)orig;
 int64_t value;
 if(type==ARM_ABS32){
  value=(int64_t)s+a;
  if(value<0||value>UINT32_MAX)return SPMP_OV_BAD_RELOCATION;
  *patched=(uint32_t)value;
 }else if(type==ARM_REL32){
  value=(int64_t)s+a-(int64_t)place;
  if(value<INT32_MIN||value>INT32_MAX)return SPMP_OV_BAD_RELOCATION;
  *patched=(uint32_t)(int32_t)value;
 }else if(type==ARM_CALL||type==ARM_JUMP24||type==ARM_PC24){
  if((orig&0x0e000000u)!=0x0a000000u)return SPMP_OV_BAD_RELOCATION;
  int32_t encoded=(int32_t)((orig&0x00ffffffu)<<8)>>6;
  value=(int64_t)s+(int64_t)encoded-(int64_t)place;
  if(value<-(1LL<<25)||value>= (1LL<<25)|| (value&3))return SPMP_OV_BAD_RELOCATION;
  *patched=(orig&0xff000000u)|(((uint32_t)(value>>2))&0x00ffffffu);
 }else if(type==ARM_PREL31){
  int32_t encoded=(int32_t)((orig&0x7fffffffu)<<1)>>1;
  value=(int64_t)s+encoded-(int64_t)place;
  if(value<-(1LL<<30)||value>=(1LL<<30))return SPMP_OV_BAD_RELOCATION;
  *patched=(orig&0x80000000u)|((uint32_t)value&0x7fffffffu);
 }else return SPMP_OV_UNSUPPORTED;
 return 0;
}
static int compute_plan(plan *p){
 if(!p->elf||p->length<52||!p->cap||!p->base)return SPMP_OV_BAD_INPUT;
 const uint8_t *e=p->elf;
 if(memcmp(e,"\177ELF",4)||e[4]!=1||e[5]!=1||e[6]!=1||r16(e+16)!=1||r16(e+18)!=40||
    r32(e+20)!=1||r16(e+40)!=52||r16(e+46)!=40||
    (r32(e+36)&0xff000000u)!=0x05000000u)return SPMP_OV_BAD_ELF;
 p->num=r16(e+48);p->shstr=r16(e+50);p->shtoff=r32(e+32);
 if(!p->num||p->num>SPMP_OVERLAY_MAX_SECTIONS||p->shstr>=p->num||
    !bounds(p->length,p->shtoff,(uint64_t)p->num*40))return SPMP_OV_BAD_ELF;
 shdr str;section(p,p->shstr,&str);
 if(str.type!=SHT_STRTAB||!bounds(p->length,str.off,str.size))return SPMP_OV_BAD_ELF;
 const uint8_t *names=p->elf+str.off;
 size_t cursor=0;uint32_t meta_found=0,entry_found=0;
 for(uint16_t i=0;i<p->num;i++){
  shdr s;section(p,i,&s);
  if(s.type!=SHT_NOBITS && s.type!=SHT_NULL && !bounds(p->length,s.off,s.size))return SPMP_OV_BAD_ELF;
  if(s.flags&SHF_ALLOC){
   if(s.type!=SHT_PROGBITS&&s.type!=SHT_NOBITS&&s.type!=0x70000001u)return SPMP_OV_UNSUPPORTED;
   uint32_t al=s.align?s.align:1;
   if(al>4096 || (al&(al-1)))return SPMP_OV_BAD_ELF;
   if(cursor>SIZE_MAX-(al-1u))return SPMP_OV_OVERFLOW;
   cursor=(cursor+(al-1u))&~(size_t)(al-1u);
   if(cursor>p->cap||s.size>p->cap-cursor||cursor>UINT32_MAX)return SPMP_OV_NO_MEMORY;
   p->allocated[i]=1;p->offs[i]=(uint32_t)cursor;
   cursor+=s.size;p->image.loaded_sections++;
   if(name_eq(names,str.size,r32(p->elf+p->shtoff+(size_t)i*40),".spmp.meta")){
    if(meta_found++ || s.type!=SHT_PROGBITS||s.size!=sizeof(spmp_module_meta))return SPMP_OV_BAD_META;
    memcpy(&p->image.meta,p->elf+s.off,sizeof(spmp_module_meta));
   }
  }
 }
 p->total=cursor;
 if(!meta_found||p->image.meta.magic!=SPMP_OVERLAY_MAGIC ||
    p->image.meta.abi_version!=SPMP_OVERLAY_ABI ||
    p->image.meta.core_api_version!=SPMP_CORE_API_VERSION ||p->image.meta.target_arm_isa!=5 ||
    !p->image.meta.core_id[0] || !memchr(p->image.meta.core_id,0,SPMP_OVERLAY_ID_LEN)||
    p->image.meta.max_game_bytes>SPMP_OVERLAY_MAX_BYTES||
    p->image.meta.reserved[0]||p->image.meta.reserved[1])return SPMP_OV_BAD_META;
 if((uint64_t)p->base+cursor>UINT32_MAX)return SPMP_OV_OVERFLOW;
 for(uint16_t i=0;i<p->num;i++){
  shdr s;section(p,i,&s);
  if(s.type==SHT_RELA)return SPMP_OV_UNSUPPORTED;
  if(s.type==SHT_SYMTAB){
   shdr strings; if(symtab_for(p,&s,&strings))return SPMP_OV_BAD_ELF;
   for(uint32_t k=0;k<s.size/16;k++){
    const uint8_t *q=p->elf+s.off+(size_t)k*16;
    uint32_t name=r32(q);
    if(name_eq(p->elf+strings.off,strings.size,name,"spmp_module_entry_v1")){
     uint32_t v,n;int rc=read_symbol(p,&s,k,&v,&n,&strings);
     if(rc)return rc;
     shdr sec;section(p,r16(q+14),&sec);
     if(!(sec.flags&SHF_EXEC)||r32(q+4)>=sec.size||entry_found++)return SPMP_OV_BAD_SYMBOL;
     p->image.entry_vma=v;
    }
   }
  }
  if(s.type==SHT_REL){
   if(s.entsize!=8||s.size%8||s.info>=p->num||s.link>=p->num||!p->allocated[s.info])return SPMP_OV_BAD_RELOCATION;
   shdr tab;section(p,(uint16_t)s.link,&tab);
   if(tab.type!=SHT_SYMTAB)return SPMP_OV_BAD_RELOCATION;
   for(uint32_t k=0;k<s.size/8;k++){
    uint32_t patched;int rc=relocation(p,&s,p->elf+s.off+(size_t)k*8,&patched);
    if(rc)return rc;
    p->relocs++;
   }
  }
 }
 if(entry_found!=1)return SPMP_OV_BAD_SYMBOL;
 p->image.used_bytes=p->total;p->image.relocation_count=p->relocs;
 /* Identity for an updated binary with the same core_id. Not a cryptographic
  * authenticity check; future signed packaging must verify trust separately. */
 uint32_t hash=2166136261u;
 for(size_t n=0;n<p->length;n++)hash=(hash ^ p->elf[n])*16777619u;
 p->image.source_fingerprint=hash;
 return 0;
}
static int prepare(plan *p,const uint8_t *elf,size_t bytes,size_t cap,const spmp_overlay_options *opt){
 if(!p||!opt||cap>SPMP_OVERLAY_MAX_BYTES)return SPMP_OV_BAD_INPUT;
 memset(p,0,sizeof *p);p->elf=elf;p->length=bytes;p->cap=cap;p->base=opt->virtual_base;
 if(p->base & 0xfffu) return SPMP_OV_BAD_INPUT; /* aligned simulated/real overlay VMA */
 return compute_plan(p);
}
int spmp_overlay_probe(const uint8_t *elf,size_t elf_size,size_t slot_size,
 const spmp_overlay_options *options,spmp_overlay_image *out){
 if(!out)return SPMP_OV_BAD_INPUT;
 plan p;int rc=prepare(&p,elf,elf_size,slot_size,options);
 if(rc)return rc;
 *out=p.image;return 0;
}
int spmp_overlay_load(const uint8_t *elf,size_t elf_size,void *slot,
 size_t slot_size,const spmp_overlay_options *options,spmp_overlay_image *out){
 if(!slot||!out||!options)return SPMP_OV_BAD_INPUT;
 plan p;int rc=prepare(&p,elf,elf_size,slot_size,options);if(rc)return rc;
 uint8_t *dst=(uint8_t *)slot;
 /* Input/output overlap is forbidden, including when file buffer lies in slot. */
 uintptr_t a=(uintptr_t)elf,b=(uintptr_t)dst;
 if(a<=UINTPTR_MAX-elf_size && b<=UINTPTR_MAX-slot_size &&
    a<b+slot_size && b<a+elf_size)return SPMP_OV_BAD_INPUT;
 memset(dst,0,p.total);
 for(uint16_t i=0;i<p.num;i++)if(p.allocated[i]){
  shdr s;section(&p,i,&s);
  if(s.type!=SHT_NOBITS&&s.size)memcpy(dst+p.offs[i],elf+s.off,s.size);
 }
 for(uint16_t i=0;i<p.num;i++){
  shdr s;section(&p,i,&s);if(s.type!=SHT_REL)continue;
  for(uint32_t j=0;j<s.size/8;j++){
   const uint8_t *q=elf+s.off+(size_t)j*8;
   uint32_t val;rc=relocation(&p,&s,q,&val);if(rc){memset(dst,0,p.total);return rc;}
   w32(dst+p.offs[s.info]+r32(q),val);
  }
 }
 if(options->dcache_clean && options->icache_invalidate){
  options->dcache_clean(options->cache_ctx,dst,p.total);
  options->icache_invalidate(options->cache_ctx,dst,p.total);
  p.image.executable=1;
 }
 *out=p.image;return 0;
}
void spmp_overlay_manager_init(spmp_overlay_manager *mgr,void *ram,
 size_t bytes,const spmp_overlay_options *opt){
 if(!mgr)return;
 memset(mgr,0,sizeof *mgr);
 mgr->slot=ram;mgr->slot_size=bytes;
 if(opt)mgr->options=*opt;
}
void spmp_overlay_manager_unload(spmp_overlay_manager *mgr){
 if(!mgr)return;
 if(mgr->loaded && mgr->slot)memset(mgr->slot,0,mgr->resident.used_bytes);
 mgr->loaded=0;memset(&mgr->resident,0,sizeof mgr->resident);
}
int spmp_overlay_manager_switch(spmp_overlay_manager *mgr,
 const uint8_t *elf,size_t bytes,spmp_overlay_image *out){
 if(!mgr||!mgr->slot)return SPMP_OV_BAD_INPUT;
 spmp_overlay_image proposed;
 int rc=spmp_overlay_probe(elf,bytes,mgr->slot_size,&mgr->options,&proposed);
 if(rc)return rc; /* invalid module cannot destroy current resident */
 if(mgr->loaded && !strcmp(mgr->resident.meta.core_id,proposed.meta.core_id) &&
    mgr->resident.source_fingerprint==proposed.source_fingerprint){
  if(out)*out=mgr->resident;
  return 1; /* cache hit: callers should change ROM, not reload code */
 }
 spmp_overlay_manager_unload(mgr);
 rc=spmp_overlay_load(elf,bytes,mgr->slot,mgr->slot_size,&mgr->options,&proposed);
 if(rc){spmp_overlay_manager_unload(mgr);return rc;}
 mgr->resident=proposed;mgr->loaded=1;mgr->generations++;
 if(out)*out=proposed;
 return 0;
}
const char *spmp_overlay_error(int status){
 switch(status){case 0:return "ok";case 1:return "resident module reused";
 case SPMP_OV_BAD_INPUT:return "bad arguments";
 case SPMP_OV_BAD_ELF:return "unsupported or corrupt ELF";
 case SPMP_OV_OVERFLOW:return "address overflow";
 case SPMP_OV_NO_MEMORY:return "module exceeds RAM slot";
 case SPMP_OV_BAD_META:return "invalid SPMP module metadata";
 case SPMP_OV_BAD_SYMBOL:return "undefined or unsafe symbol";
 case SPMP_OV_BAD_RELOCATION:return "invalid relocation";
 case SPMP_OV_UNSUPPORTED:return "unsupported section/relocation";
 case SPMP_OV_NOT_READY:return "cache synchronization required";
 default:return "unknown loader error";}
}
