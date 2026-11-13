/* SPMP-OS Stage17: host development adapter for the actual Libretro ABI.
 * Two independent modules share callbacks because only one is active.
 * On ARMv5, replace dlopen with the SPMP overlay loader and do NOT use this host file.
 * No fabricated ROM emulation; missing PicoDrive is an explicit failure.
 */
#include "spmp_cores.h"
#include "spmp_md_rom.h"
#include "spmp_small_video.h"
#include "third_party/snes9x/libretro/libretro.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <limits.h>

#define LR_MAX_ROM (16u*1024u*1024u)
#define LR_MAX_W 512u
#define LR_MAX_H 512u
#define LR_MAX_SAVE (2u*1024u*1024u)

typedef struct {
 void (*set_env)(retro_environment_t);
 void (*set_video)(retro_video_refresh_t);
 void (*set_audio_one)(retro_audio_sample_t);
 void (*set_audio_batch)(retro_audio_sample_batch_t);
 void (*set_poll)(retro_input_poll_t);
 void (*set_state)(retro_input_state_t);
 void (*init)(void);
 void (*deinit)(void);
 void (*get_system_info)(struct retro_system_info *);
 void (*get_av)(struct retro_system_av_info *);
 bool (*load_game)(const struct retro_game_info *);
 void (*unload_game)(void);
 void (*run)(void);
 void *(*memory_data)(unsigned);
 size_t (*memory_size)(unsigned);
 size_t (*serialize_size)(void);
 bool (*serialize)(void*,size_t);
 bool (*unserialize)(const void*,size_t);
} lr_exports;

typedef struct {
 const char *id, *fallback;
 void *handle;
 lr_exports f;
 const spmp_core_host *host;
 uint8_t *rom;
 char *game_name;
 unsigned sample_rate, format;
 unsigned width, height;
 unsigned frames;
 uint32_t polled_buttons;
 unsigned input_polls;
 unsigned video_callbacks, skipped_video, frameskip;
 size_t rom_capacity, frame_capacity;
 int initialized, loaded;
 uint16_t *frame;
} lr_slot;
static lr_slot slots[3]={
 {.id="snes", .fallback="third_party/snes9x/libretro/snes9x_libretro.so"},
 {.id="megadrive", .fallback="third_party/picodrive/picodrive_libretro.so"},
 {.id="gba", .fallback="third_party/gpsp/gpsp_libretro.so"}
};
static lr_slot *active;

/* POSIX guarantees dlsym() can resolve code symbols; memcpy avoids ISO C
 * function/data pointer cast warnings on common ELF host platforms. */
#define LR_SYM(slot,member,name) do { \
 void *symbol_address=dlsym((slot)->handle,(name)); \
 if(!symbol_address || sizeof(symbol_address)!=sizeof((slot)->f.member)) goto failed; \
 memcpy(&(slot)->f.member,&symbol_address,sizeof(symbol_address)); \
} while(0)
static lr_slot *slot_for(unsigned i){return i<3?&slots[i]:NULL;}
static int module_open(unsigned i){
 lr_slot *s=slot_for(i);
 if(!s)return -1;
 if(s->handle)return 0;
 const char *custom=getenv(i==2?"SPMP_GPSP_SO":(i==1?"SPMP_PICODRIVE_SO":"SPMP_SNES9X_SO"));
 const char *path=custom&&*custom?custom:s->fallback;
 s->handle=dlopen(path,RTLD_NOW|RTLD_LOCAL);
 if(!s->handle){fprintf(stderr,"SPMP %s: missing module %s: %s\n",s->id,path,dlerror());return -1;}
 LR_SYM(s,set_env,"retro_set_environment");
 LR_SYM(s,set_video,"retro_set_video_refresh");
 LR_SYM(s,set_audio_one,"retro_set_audio_sample");
 LR_SYM(s,set_audio_batch,"retro_set_audio_sample_batch");
 LR_SYM(s,set_poll,"retro_set_input_poll");
 LR_SYM(s,set_state,"retro_set_input_state");
 LR_SYM(s,init,"retro_init");
 LR_SYM(s,deinit,"retro_deinit");
 LR_SYM(s,get_system_info,"retro_get_system_info");
 LR_SYM(s,get_av,"retro_get_system_av_info");
 LR_SYM(s,load_game,"retro_load_game");
 LR_SYM(s,unload_game,"retro_unload_game");
 LR_SYM(s,run,"retro_run");
 LR_SYM(s,memory_data,"retro_get_memory_data");
 LR_SYM(s,memory_size,"retro_get_memory_size");
 LR_SYM(s,serialize_size,"retro_serialize_size");
 LR_SYM(s,serialize,"retro_serialize");
 LR_SYM(s,unserialize,"retro_unserialize");
 return 0;
failed:
 fprintf(stderr,"SPMP %s: incomplete Libretro symbols (%s)\n",s->id,dlerror());
 dlclose(s->handle);s->handle=NULL;memset(&s->f,0,sizeof(s->f));return -1;
}
static void module_close(unsigned i){
 lr_slot *s=slot_for(i);if(!s||!s->handle)return;
 if(s->loaded)return; /* never unmap live code */
 if(s->initialized){s->f.deinit();s->initialized=0;}
 dlclose(s->handle);s->handle=NULL;memset(&s->f,0,sizeof(s->f));
 if(active==s)active=NULL;
}
static void lr_log(enum retro_log_level level,const char *fmt,...){
 (void)level;va_list ap;va_start(ap,fmt);
 fprintf(stderr,"SPMP libretro: ");vfprintf(stderr,fmt,ap);
 va_end(ap);
}
static bool lr_environment(unsigned cmd,void *p){
 if(!active)return false;
 switch(cmd){
 case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
  /* Snes9x may log binary cartridge header bytes. Let it use its own fallback. */
  if(p&&active&&!strcmp(active->id,"megadrive")){((struct retro_log_callback*)p)->log=lr_log;return true;}return false;
 case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
  if(!p)return false;
  if(*(const enum retro_pixel_format*)p>RETRO_PIXEL_FORMAT_RGB565)return false;
  active->format=*(const enum retro_pixel_format*)p;return true;
 case RETRO_ENVIRONMENT_GET_CAN_DUPE:
  if(p)*(bool*)p=true;
  return p!=NULL;
 case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
  if(p)*(bool*)p=false;
  return p!=NULL;
 case RETRO_ENVIRONMENT_GET_VARIABLE:
  if(p){((struct retro_variable*)p)->value=NULL;return false;}
  return false;
 case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
 case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
  if(p){
   /* Host harness only. The ARM firmware uses a HAL-provided system directory. */
   const char *dir=getenv("SPMP_BIOS_DIR");
   *(const char**)p=(dir && *dir)?dir:".";
   return true;
  }return false;
 case RETRO_ENVIRONMENT_SET_VARIABLES:
 case RETRO_ENVIRONMENT_SET_CORE_OPTIONS:
 case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL:
 case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
 case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
 case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
 case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
 case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
 case RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS:
 case RETRO_ENVIRONMENT_SET_GEOMETRY:
  return true;
 default:return false;
 }
}
static void lr_video(const void *data,unsigned w,unsigned h,size_t pitch){
 lr_slot *s=active;
 if(!s||!s->host||!s->host->present||!w||!h||w>LR_MAX_W||h>LR_MAX_H)return;
 if(!data){if(s->frame&&s->width&&s->height)s->host->present(s->host->user,s->frame,s->width,s->height);return;}
 /* Frameskip affects ONLY expensive frame conversion/presentation.
  * CPU, timers, sound and memory continue to run every emulated frame. */
 s->video_callbacks++;
 if(s->frameskip && (s->video_callbacks-1u)%(s->frameskip+1u)!=0u){
  s->skipped_video++;return;
 }
 /* The SPMP8000 LCD is 320x240: never allocate a full 512x448
  * HiRes SNES output buffer when a 320x240-or-smaller frame suffices.
  * This conversion is also used for an exact source-sized RGB565 image. */
 unsigned out_w=0,out_h=0;
 if(spmp_video_fit(w,h,320u,240u,&out_w,&out_h))return;
 const size_t needed=(size_t)out_w*out_h;
 if(needed>s->frame_capacity){
  uint16_t *new_frame=realloc(s->frame,needed*sizeof(uint16_t));
  if(!new_frame)return;
  s->frame=new_frame;s->frame_capacity=needed;
 }
 const unsigned format=s->format==RETRO_PIXEL_FORMAT_XRGB8888?
   SPMP_VIDEO_XRGB8888:(s->format==RETRO_PIXEL_FORMAT_RGB565?
   SPMP_VIDEO_RGB565:SPMP_VIDEO_0RGB1555);
 if(spmp_video_convert(data,pitch,w,h,format,s->frame,out_w,out_h))return;
 s->width=out_w;s->height=out_h;s->frames++;
 s->host->present(s->host->user,s->frame,out_w,out_h);
}
static size_t lr_audio_batch(const int16_t *data,size_t n){
 if(active&&active->host&&active->host->audio_pcm&&data&&n)
  active->host->audio_pcm(active->host->user,data,n,active->sample_rate?active->sample_rate:32000u);
 return n;
}
static void lr_audio_one(int16_t l,int16_t r){int16_t sample[2]={l,r};(void)lr_audio_batch(sample,1);}
static void lr_input_poll(void){
 lr_slot *s=active;
 if(s){s->input_polls++;s->polled_buttons=s->host&&s->host->buttons?
  s->host->buttons(s->host->user):0;}
}
static int16_t lr_input_state(unsigned port,unsigned device,unsigned index,unsigned id){
 if(!active||!active->host||!active->host->buttons||port||index||device!=RETRO_DEVICE_JOYPAD)return 0;
 uint32_t mask=active->polled_buttons;
 unsigned bit;
 switch(id){
 case RETRO_DEVICE_ID_JOYPAD_UP:bit=0;break;
 case RETRO_DEVICE_ID_JOYPAD_DOWN:bit=1;break;
 case RETRO_DEVICE_ID_JOYPAD_LEFT:bit=2;break;
 case RETRO_DEVICE_ID_JOYPAD_RIGHT:bit=3;break;
 case RETRO_DEVICE_ID_JOYPAD_B:bit=4;break;
 case RETRO_DEVICE_ID_JOYPAD_A:bit=5;break;
 case RETRO_DEVICE_ID_JOYPAD_START:bit=6;break;
 case RETRO_DEVICE_ID_JOYPAD_SELECT:bit=7;break;
 case RETRO_DEVICE_ID_JOYPAD_X:bit=8;break;
 case RETRO_DEVICE_ID_JOYPAD_Y:bit=9;break;
 case RETRO_DEVICE_ID_JOYPAD_L:bit=10;break;
 case RETRO_DEVICE_ID_JOYPAD_R:bit=11;break;
 default:return 0;
 }
 return (mask&(1u<<bit))?1:0;
}
static char *lr_strdup(const char *p){size_t n=strlen(p)+1;char *t=malloc(n);if(t)memcpy(t,p,n);return t;}
static void save_ram(lr_slot *s){
 if(!s||!s->host||!s->host->store_save||!s->loaded)return;
 void *p=s->f.memory_data(RETRO_MEMORY_SAVE_RAM);
 size_t n=s->f.memory_size(RETRO_MEMORY_SAVE_RAM);
 if(p&&n&&n<=LR_MAX_SAVE)(void)s->host->store_save(s->host->user,s->id,s->game_name,(const uint8_t*)p,n);
}
static void restore_ram(lr_slot *s){
 if(!s||!s->host||!s->host->load_save)return;
 void *p=s->f.memory_data(RETRO_MEMORY_SAVE_RAM);
 size_t n=s->f.memory_size(RETRO_MEMORY_SAVE_RAM),loaded=0;
 if(!p||!n||n>LR_MAX_SAVE)return;
 (void)s->host->load_save(s->host->user,s->id,s->game_name,(uint8_t*)p,n,&loaded);
}
/* Sega CD content must stay on the storage device, not in an oversized ROM
 * allocation. PicoDrive's own CD reader handles sector access and CUE parsing.
 * Other cores and normal MD/SNES cartridges use the existing bounded read path.
 */
static int spmp_cd_path_only(unsigned which,const char *path,size_t file_bytes){
 const char *dot=strrchr(path,'.');
 if(which!=1 || !dot)return 0;
 dot++;
 if(!strcasecmp(dot,"cue")||!strcasecmp(dot,"iso")||
    !strcasecmp(dot,"chd")||!strcasecmp(dot,"m3u"))return 1;
 /* Binary track images can exceed cartridge limit; let the CD parser inspect
  * the header. Rejects oversized non-disc ROMs safely in the core itself. */
 if(!strcasecmp(dot,"bin") && file_bytes>LR_MAX_ROM)return 1;
 return 0;
}
static int lr_start(unsigned which,const spmp_core_host *host,const char *path){
 lr_slot *s=slot_for(which);
 if(!s||!host||!host->present||!host->read_entire||!path||!path[0]||active){fprintf(stderr,"SPMP core: bad preflight\n");return -1;}
 if(module_open(which)){fprintf(stderr,"SPMP core: module open failed\n");return -1;}
 /* Exact size preflight. Do NOT read an ISO, raw CD track or cue sheet into
  * the cartridge arena. CD streaming needs an open/read/seek VFS in the
  * eventual ARM firmware, but host PicoDrive already provides it. */
 size_t capacity=0, bytes=0;
 if(host->file_size){
  if(host->file_size(host->user,path,&capacity)||!capacity){
   fprintf(stderr,"SPMP core: file size failed\n");return -1;
  }
 } else {
  capacity=LR_MAX_ROM;
 }
 const int is_disc=spmp_cd_path_only(which,path,capacity);
 if(!is_disc && capacity>LR_MAX_ROM){
  fprintf(stderr,"SPMP core: oversized cartridge\n");return -1;
 }
 s->game_name=lr_strdup(path);
 if(!s->game_name){fprintf(stderr,"SPMP core: game name allocation failed\n");goto failed;}
 if(!is_disc){
  s->rom=malloc(capacity);
  if(!s->rom){fprintf(stderr,"SPMP core: ROM allocation failed\n");goto failed;}
  s->rom_capacity=capacity;
  if(host->read_entire(host->user,path,s->rom,capacity,&bytes)||!bytes||bytes>capacity||
     (host->file_size&&bytes!=capacity)){
    fprintf(stderr,"SPMP core: ROM read failed cap=%zu bytes=%zu\n",capacity,bytes);goto failed;
  }
  if(which==1 && spmp_md_format(s->rom,bytes)==2){
   uint8_t *scratch=malloc(16384u);
   if(!scratch)goto failed;
   int rc=spmp_md_canonicalize(s->rom,&bytes,scratch,16384u);
   free(scratch);
   if(rc)goto failed;
  }
 }
 s->host=host;active=s;s->format=RETRO_PIXEL_FORMAT_0RGB1555;
 s->polled_buttons=0;s->input_polls=0;
 s->video_callbacks=0;s->skipped_video=0;s->frameskip=0;
 s->f.set_env(lr_environment);
 s->f.set_video(lr_video);
 s->f.set_audio_one(lr_audio_one);
 s->f.set_audio_batch(lr_audio_batch);
 s->f.set_poll(lr_input_poll);
 s->f.set_state(lr_input_state);
 if(!s->initialized){s->f.init();s->initialized=1;}
 struct retro_system_info info={0};s->f.get_system_info(&info);
 struct retro_game_info game={0};game.path=path;game.data=(is_disc||info.need_fullpath)?NULL:s->rom;game.size=is_disc?0:bytes;
 if(!s->f.load_game(&game)){fprintf(stderr,"SPMP %s: retro_load_game failed for %s\n",s->id,path);goto failed;}
 s->loaded=1;
 struct retro_system_av_info av={0};s->f.get_av(&av);
 if(av.timing.sample_rate>1000.0&&av.timing.sample_rate<192001.0)s->sample_rate=(unsigned)av.timing.sample_rate;
 restore_ram(s);return 0;
failed:
 if(active==s)active=NULL;
 free(s->rom);s->rom=NULL;s->rom_capacity=0;
 free(s->frame);s->frame=NULL;s->frame_capacity=0;
 free(s->game_name);s->game_name=NULL;s->host=NULL;
 return -1;
}
static int lr_tick(void){
 lr_slot *s=active;if(!s||!s->loaded)return -1;
 s->f.run();return 0;
}
static void lr_stop(void){
 lr_slot *s=active;if(!s)return;
 save_ram(s);
 if(s->loaded){s->f.unload_game();s->loaded=0;}
 free(s->rom);s->rom=NULL;s->rom_capacity=0;
 free(s->frame);s->frame=NULL;s->frame_capacity=0;
 free(s->game_name);s->game_name=NULL;s->host=NULL;
 s->width=0;s->height=0;active=NULL;
}
static size_t lr_state_size(void){return active&&active->loaded?active->f.serialize_size():0;}
static int lr_serialize(void *p,size_t n){
 return active&&active->loaded&&p&&n==lr_state_size()&&active->f.serialize(p,n)?0:-1;
}
static int lr_unserialize(const void *p,size_t n){
 return active&&active->loaded&&p&&n==lr_state_size()&&active->f.unserialize(p,n)?0:-1;
}
static int lr_option(unsigned id,int value){
 if(!active)return 1;
 /* SPMP_OPT_FRAMESKIP=4: 0 off, 1 auto (scheduler not yet ready),
  * 2 skip one video frame, 3 skip two video frames. */
 if(id!=4u)return 1;
 if(value==0){active->frameskip=0;return 0;}
 if(value==1)return 1;
 if(value==2||value==3){active->frameskip=(unsigned)value-1u;return 0;}
 return -1;
}
static int snes_start(const spmp_core_host *h,const char *p){return lr_start(0,h,p);}
static int md_start(const spmp_core_host *h,const char *p){return lr_start(1,h,p);}
static int gba_start(const spmp_core_host *h,const char *p){return lr_start(2,h,p);}
const spmp_core_ops spmp_snes_ops={snes_start,lr_tick,lr_stop,lr_state_size,lr_serialize,lr_unserialize,lr_option};
const spmp_core_ops spmp_megadrive_ops={md_start,lr_tick,lr_stop,lr_state_size,lr_serialize,lr_unserialize,lr_option};
const spmp_core_ops spmp_gba_ops={gba_start,lr_tick,lr_stop,lr_state_size,lr_serialize,lr_unserialize,lr_option};
int spmp_libretro_module_open(unsigned which){return module_open(which);}
void spmp_libretro_module_close(unsigned which){module_close(which);}
unsigned spmp_libretro_last_frames(unsigned which){lr_slot*s=slot_for(which);return s?s->frames:0;}

/* Development-only counters; these report allocation policy, not free RAM. */
size_t spmp_libretro_rom_capacity(unsigned which){lr_slot*s=slot_for(which);return s?s->rom_capacity:0;}
size_t spmp_libretro_frame_bytes(unsigned which){lr_slot*s=slot_for(which);return s?s->frame_capacity*sizeof(uint16_t):0;}
unsigned spmp_libretro_input_polls(unsigned which){lr_slot*s=slot_for(which);return s?s->input_polls:0;}
unsigned spmp_libretro_skipped_video(unsigned which){lr_slot*s=slot_for(which);return s?s->skipped_video:0;}
