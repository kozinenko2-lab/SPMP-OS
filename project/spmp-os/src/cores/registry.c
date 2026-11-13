#include "spmp_cores.h"
#include "spmp_md_rom.h"
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>

extern const spmp_core_ops spmp_diagnostic_ops;
extern const spmp_core_ops spmp_nes_ops;
extern const spmp_core_ops spmp_fceumm_ops;
extern const spmp_core_ops spmp_chip8_ops;
extern const spmp_core_ops spmp_gambatte_ops;
extern int spmp_gambatte_module_open(void);
extern void spmp_gambatte_module_close(void);
extern const spmp_core_ops spmp_pcengine_ops;
extern const spmp_core_ops spmp_atari2600_ops;
extern const spmp_core_ops spmp_sms_ops;
extern int spmp_sms_module_open(void);
extern void spmp_sms_module_close(void);
extern const spmp_core_ops spmp_snes_ops,spmp_megadrive_ops,spmp_gba_ops;
extern int spmp_libretro_module_open(unsigned which);
extern void spmp_libretro_module_close(unsigned which);
/* Source archives are audit references only, not linked code. Do not mark
 * them runnable until a native adapter has been compiled AND tested. */
static const spmp_core_desc cores[]={
 {"diagnostic","SPMP-OS RGB565 self-test","SPMP-OS internal","-",SPMP_CORE_HOST_TESTED,&spmp_diagnostic_ops},
 {"chip8","CHIP-8 interpreter","SPMP-OS original C core","ch8;c8",SPMP_CORE_HOST_TESTED,&spmp_chip8_ops},
 {"nes","NES / Famicom (FCEUmm)","User-supplied libretro-fceumm-master.zip, GPL-2.0", "nes;fds;unf;unif", SPMP_CORE_HOST_TESTED,&spmp_fceumm_ops},
 {"nes-baseline","NES limited test core (mapper 0/2/3)","SPMP-OS Stage7 baseline", "-", SPMP_CORE_HOST_TESTED,&spmp_nes_ops},
 {"gb","Game Boy / Color (Gambatte)","User Gambatte GPLv2, dynamically linked on host","gb;gbc;dmg",SPMP_CORE_HOST_TESTED,&spmp_gambatte_ops},
 {"gba","Game Boy Advance (gpSP interpreter)","user-supplied gpsp-master.zip; host libretro interpreter","gba",SPMP_CORE_HOST_TESTED,&spmp_gba_ops},
 {"snes","Super NES","user-supplied mainline Snes9x; host libretro adapter (non-commercial)","sfc;smc;fig;swc",SPMP_CORE_HOST_TESTED,&spmp_snes_ops},
 {"megadrive","Mega Drive / Genesis","User PicoDrive source; real host-tested Mega Drive core (non-commercial)","md;gen;smd;32x;cue;iso;chd;m3u",SPMP_CORE_HOST_TESTED,&spmp_megadrive_ops},
 {"doom","Doom","libspmp8k-master.zip:demos/doom110","wad",SPMP_CORE_SOURCE_AVAILABLE,0},
 {"quake","Quake I","binary only: spmp8k-quake_beta1.bin","pak",SPMP_CORE_SOURCE_UNVERIFIED,0},
 {"spectrum","ZX Spectrum","xpectrum-spmp8000-master.zip","z80;sna;tzx;tap",SPMP_CORE_SOURCE_AVAILABLE,0},
 {"atari2600","Atari 2600 (z26)","z26 GPLv2 native SPMP adapter","a26;bin",SPMP_CORE_HOST_TESTED,&spmp_atari2600_ops},
 {"pcengine","PC Engine / TurboGrafx","TGEmu GPLv2 (integrated)","pce",SPMP_CORE_HOST_TESTED,&spmp_pcengine_ops},
 {"c64","Commodore 64 (Frodo)","Frodo4-spmp8000-master.zip","d64;t64;prg;tap",SPMP_CORE_SOURCE_AVAILABLE,0},
 {"amstrad","Amstrad CPC (Caprice)","wiituka-spmp8000-master.zip","dsk;cdt;sna",SPMP_CORE_SOURCE_AVAILABLE,0},
 {"msx","MSX","binary only: spmp8k-msx_r36.bin","dsk;cas;rom",SPMP_CORE_SOURCE_UNVERIFIED,0},
 {"sms","Sega Master System / Game Gear","Retro-Go SMS Plus GX / GPL-2.0", "sms;gg;sg",SPMP_CORE_HOST_TESTED,&spmp_sms_ops},
 {"scummvm","ScummVM","scummvm-spmp8000-master.zip (SPMP backend not found)","scummvm",SPMP_CORE_SOURCE_AVAILABLE,0},
 {"xrick","Rick Dangerous","xrick-spmp8000-master.zip","-",SPMP_CORE_SOURCE_AVAILABLE,0},
 {"ps1","PlayStation research (psx4all)","PSX4ALL Stage14 ARMv5 ET_REL staged; NOT linked/runnable","cue;chd;bin",SPMP_CORE_SOURCE_UNVERIFIED,0}
};
static const spmp_core_desc *running;
/* Residency is logical for static cores; only Gambatte host code physically
 * dlopens/dlcloses. ARM overlay loading is still unimplemented. */
static const spmp_core_desc *cached;
static char *current_content;
static int paused;
static const spmp_core_host *current_host;
static char *clone_content(const char *s){
 if(!s)return NULL;
 size_t n=strlen(s)+1;
 if(n>4096)return NULL;
 char *p=(char*)malloc(n);
 if(p)memcpy(p,s,n);
 return p;
}
static int content_equal(const char *a,const char *b){
 return (a==NULL&&b==NULL)||(a&&b&&!strcmp(a,b));
}
size_t spmp_core_count(void){return sizeof cores/sizeof cores[0];}
const spmp_core_desc *spmp_core_at(size_t i){return i<spmp_core_count()?&cores[i]:0;}
const spmp_core_desc *spmp_core_find(const char *id){
 if(!id)return 0;
 for(size_t i=0;i<spmp_core_count();i++)if(strcmp(id,cores[i].id)==0)return &cores[i];
 return 0;
}
static int has_extension(const char *allowed,const char *actual){
 if(!allowed||!actual||!strcmp(allowed,"-"))return 0;
 const char *p=allowed;
 while(*p){
  const char *end=strchr(p,';');if(!end)end=p+strlen(p);
  if((size_t)(end-p)==strlen(actual)){
   size_t i=0;
   for(;i<(size_t)(end-p);i++)if(tolower((unsigned char)p[i])!=tolower((unsigned char)actual[i]))break;
   if(i==(size_t)(end-p))return 1;
  }
  p=*end?end+1:end;
 }
 return 0;
}
spmp_match_result spmp_core_resolve(const char *file,
  const spmp_core_desc **out,size_t capacity,size_t *total){
 if(total)*total=0;
 if(!file)return SPMP_MATCH_NONE;
 const char *slash=strrchr(file,'/'),*backslash=strrchr(file,'\\');
 const char *base=file;
 if(slash&&slash>=base)base=slash+1;
 if(backslash&&backslash>=base)base=backslash+1;
 const char *dot=strrchr(base,'.');
 if(!dot||!dot[1])return SPMP_MATCH_NONE;
 const char *ext=dot+1;
 size_t matches=0;
 for(size_t i=0;i<spmp_core_count();i++){
  if(has_extension(cores[i].content_extensions,ext)){
   if(out&&matches<capacity)out[matches]=&cores[i];
   matches++;
  }
 }
 if(total)*total=matches;
 return matches==0?SPMP_MATCH_NONE:matches==1?SPMP_MATCH_UNIQUE:SPMP_MATCH_AMBIGUOUS;
}
static int acquire(const spmp_core_desc *desc){
 if(!desc||!desc->ops)return -1;
 if(cached==desc)return 0;
 if(cached){if(!strcmp(cached->id,"gb"))spmp_gambatte_module_close();
  if(!strcmp(cached->id,"sms"))spmp_sms_module_close();
  if(!strcmp(cached->id,"snes"))spmp_libretro_module_close(0);
  if(!strcmp(cached->id,"megadrive"))spmp_libretro_module_close(1);
  if(!strcmp(cached->id,"gba"))spmp_libretro_module_close(2);
  cached=NULL;}
 if(!strcmp(desc->id,"gb")&&spmp_gambatte_module_open())return -1;
 if(!strcmp(desc->id,"sms")&&spmp_sms_module_open())return -1;
 if(!strcmp(desc->id,"snes")&&spmp_libretro_module_open(0))return -1;
 if(!strcmp(desc->id,"megadrive")&&spmp_libretro_module_open(1))return -1;
 if(!strcmp(desc->id,"gba")&&spmp_libretro_module_open(2))return -1;
 cached=desc;return 0;
}
int spmp_core_start(const char *id,const spmp_core_host *host,const char *content){
 const spmp_core_desc *desc=spmp_core_find(id);
 if(running||!desc||!desc->ops||!host||!host->present)return -1;
 char *saved=clone_content(content);
 if(content&&!saved)return -1;
 if(acquire(desc)){free(saved);return -1;}
 if(desc->ops->start(host,content)){free(saved);return -1;}
 current_content=saved;
 running=desc;current_host=host;paused=0;
 return 0;
}
int spmp_core_launch(const char *id,const spmp_core_host *host,const char *content){
 const spmp_core_desc *desc=spmp_core_find(id);
 if(!desc||!desc->ops||!host||!host->present)return -1;
 /* Returning from the menu to the exact same running game is a resume,
  * not an implicit reset or a new cartridge load. */
 if(running==desc && current_host==host && content_equal(current_content,content)){
  paused=0;return 0;
 }
 /* Validate new cart existence and bounded size without mutating the current
  * session. A missing ROM must never unload a paused different platform.
  * HAL's file_size is optional for legacy cores; actual device must supply it. */
 if(content && host->file_size &&
    (!strcmp(desc->id,"megadrive") || !strcmp(desc->id,"snes"))){
  size_t bytes=0;
  if(host->file_size(host->user,content,&bytes)!=0 || bytes==0)return -1;
  /* Cartridges must fit in the ROM arena; CD images are streamed through the
   * filesystem API and may be much larger. Do not infer a CD from generic
   * .bin unless it exceeds the 16 MiB cartridge budget. */
  const char *ext=strrchr(content,'.');
  const int md=!strcmp(desc->id,"megadrive");
  const int is_disc=md && ext && (
    !strcasecmp(ext,".cue") || !strcasecmp(ext,".iso") ||
    !strcasecmp(ext,".chd") || !strcasecmp(ext,".m3u") ||
    (!strcasecmp(ext,".bin") && bytes>16u*1024u*1024u));
  if(!is_disc && bytes>16u*1024u*1024u)return -1;
  if(is_disc && (bytes>0x7fffffffu ||
     ((!strcasecmp(ext,".cue")||!strcasecmp(ext,".m3u")) && bytes>256u*1024u)))return -1;
 }
 /* Host-only two-phase preflight: missing shared library MUST NOT destroy
  * a paused game. Native ARM overlay needs storage-only validation first. */
 if(desc!=cached && !strcmp(desc->id,"megadrive") && spmp_libretro_module_open(1))return -1;
 if(desc!=cached && !strcmp(desc->id,"snes") && spmp_libretro_module_open(0))return -1;
 if(desc!=cached && !strcmp(desc->id,"gba") && spmp_libretro_module_open(2))return -1;
 spmp_core_stop();
 return spmp_core_start(id,host,content);
}
int spmp_core_pause_to_menu(void){
 if(!running)return -1;
 paused=1;return 0;
}
int spmp_core_resume(void){
 if(!running)return -1;
 paused=0;return 0;
}
int spmp_core_is_paused(void){return paused;}
int spmp_core_tick(void){
 if(!running)return -1;
 if(paused)return 2; /* Paused intentionally: do not emulate/audio. */
 return running->ops->tick();
}
void spmp_core_stop(void){
 if(running){running->ops->stop();running=NULL;}
 free(current_content);current_content=NULL;current_host=NULL;paused=0;
}
const char *spmp_core_cached_id(void){return cached?cached->id:NULL;}
size_t spmp_core_state_size(void){
 if(!running||!running->ops||!running->ops->state_size)return 0;
 return running->ops->state_size();
}
int spmp_core_serialize(void *dst,size_t len){
 if(!running||!paused||!dst||!running->ops->serialize||!len)return -1;
 return running->ops->serialize(dst,len);
}
int spmp_core_unserialize(const void *src,size_t len){
 if(!running||!paused||!src||!running->ops->unserialize||!len)return -1;
 return running->ops->unserialize(src,len);
}
int spmp_core_apply_setting(unsigned id,int value){
 if(running&&running->ops&&running->ops->set_option)
  return running->ops->set_option(id,value);
 return 1; /* persist preference but no live adapter yet */
}
void spmp_core_shutdown(void){
 spmp_core_stop();
 if(cached&&!strcmp(cached->id,"gb"))spmp_gambatte_module_close();
 if(cached&&!strcmp(cached->id,"sms"))spmp_sms_module_close();
 if(cached&&!strcmp(cached->id,"snes"))spmp_libretro_module_close(0);
 if(cached&&!strcmp(cached->id,"megadrive"))spmp_libretro_module_close(1);
 cached=NULL;
}

/* ROM signatures selected by validation of file format, not guesswork about
 * file names. Unknown .bin contents must remain ambiguous. */
static int signed_core(const char *id, const spmp_core_desc **out,
                      size_t cap, size_t *total) {
 const spmp_core_desc *core=spmp_core_find(id);
 if(!core) return SPMP_MATCH_NONE;
 if(out&&cap)out[0]=core;
 if(total)*total=1;
 return SPMP_MATCH_UNIQUE;
}
static int equal_bytes(const uint8_t *a,const char *b,size_t n){
 return memcmp(a,b,n)==0;
}
static int is_gb_logo(const uint8_t *b,size_t n){
 static const uint8_t prefix[]={0xce,0xed,0x66,0x66,0xcc,0x0d,0x00,0x0b,
   0x03,0x73,0x00,0x83,0x00,0x0c,0x00,0x0d};
 return n>=0x150 && memcmp(b+0x104,prefix,sizeof prefix)==0 &&
        b[0x147]!=0xff;
}
static int is_gba_logo(const uint8_t *b,size_t n){
 static const uint8_t prefix[]={0x24,0xff,0xae,0x51,0x69,0x9a,0xa2,0x21};
 return n>=0xc0 && memcmp(b+4,prefix,sizeof prefix)==0 && b[0xb2]==0x96;
}
static int is_sms_sega(const uint8_t *b,size_t n){
 static const size_t headers[]={0x1ff0,0x3ff0,0x7ff0};
 for(size_t i=0;i<3;i++)if(n>=headers[i]+8&&memcmp(b+headers[i],"TMR SEGA",8)==0)return 1;
 return 0;
}
spmp_match_result spmp_core_resolve_data(const char *filename,
 const uint8_t *header,size_t header_size,
 const spmp_core_desc **out,size_t out_capacity,size_t *total){
 if(total)*total=0;
 if(!header||header_size==0)return spmp_core_resolve(filename,out,out_capacity,total);
 const char *core=0;
 if(header_size>=4&&equal_bytes(header,"NES\x1a",4))core="nes";
 else if(header_size>=4&&equal_bytes(header,"FDS\x1a",4))core="nes";
 else if(header_size>=4&&equal_bytes(header,"UNIF",4))core="nes";
 else if(is_gb_logo(header,header_size))core="gb";
 else if(is_gba_logo(header,header_size))core="gba";
 else if(header_size>=0x110 &&
         (equal_bytes(header+0x100,"SEGA",4)||equal_bytes(header+0x100," SEGA",5)))core="megadrive";
 else if(spmp_md_smd_prefix(header,header_size))core="megadrive";
 else if(is_sms_sega(header,header_size))core="sms";
 /* Deliberately no arbitrary SNES/Atari or CD cue detection from bytes. */
 if(core)return (spmp_match_result)signed_core(core,out,out_capacity,total);
 return spmp_core_resolve(filename,out,out_capacity,total);
}
