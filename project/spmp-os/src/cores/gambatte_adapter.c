/* SPMP-OS Stage 9 GB/GBC adapter for the user-supplied Gambatte GPLv2 core.
 * Host demonstration: dlopen/dlclose. ARM926/RedBoot loader is NOT implemented.
 * Source: third_party/gambatte. Core remains loaded across ROM changes.
 */
#include "spmp_cores.h"
#include "libretro.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define GB_MAX_ROM (8u*1024u*1024u)
#define GB_MAX_SAVE (128u*1024u)
#define GB_MAX_RTC 256u
#define GB_MAX_W 160u
#define GB_MAX_H 144u

typedef struct {
 void (*set_env)(retro_environment_t);
 void (*set_video)(retro_video_refresh_t);
 void (*set_audio_one)(retro_audio_sample_t);
 void (*set_audio_batch)(retro_audio_sample_batch_t);
 void (*set_poll)(retro_input_poll_t);
 void (*set_state)(retro_input_state_t);
 void (*init)(void);
 void (*deinit)(void);
 bool (*load_game)(const struct retro_game_info *);
 void (*unload_game)(void);
 void (*run)(void);
 void (*get_av)(struct retro_system_av_info *);
 void *(*memory_data)(unsigned);
 size_t (*memory_size)(unsigned);
 size_t (*serialize_size)(void);
 bool (*serialize)(void *,size_t);
 bool (*unserialize)(const void *,size_t);
} gb_api;
static gb_api api;
static void *lib_handle;
static int initialized;
static int game_loaded;
static const spmp_core_host *host_api;
static char *game_path;
static unsigned audio_rate=32768;
static int rgb565_enabled;
static uint16_t frame[GB_MAX_W*GB_MAX_H];
static unsigned module_opens;
static unsigned module_closes;

/* Function pointers are loaded via memcpy to be ISO C compliant under
 * -Wpedantic; direct casting void* to a function pointer is not portable. */
#define LOAD_SYM(field,name) do { \
 void *sym=dlsym(lib_handle,name); \
 if(!sym)goto fail; \
 memcpy(&api.field,&sym,sizeof(sym)); \
} while(0)

int spmp_gambatte_module_open(void){
 if(lib_handle)return 0;
 const char *path=getenv("SPMP_GAMBATTE_SO");
 if(!path||!*path)path="third_party/gambatte/gambatte_libretro.so";
 lib_handle=dlopen(path,RTLD_NOW|RTLD_LOCAL);
 if(!lib_handle){fprintf(stderr,"Gambatte dlopen: %s\n",dlerror());return -1;}
 LOAD_SYM(set_env,"retro_set_environment");
 LOAD_SYM(set_video,"retro_set_video_refresh");
 LOAD_SYM(set_audio_one,"retro_set_audio_sample");
 LOAD_SYM(set_audio_batch,"retro_set_audio_sample_batch");
 LOAD_SYM(set_poll,"retro_set_input_poll");
 LOAD_SYM(set_state,"retro_set_input_state");
 LOAD_SYM(init,"retro_init");
 LOAD_SYM(deinit,"retro_deinit");
 LOAD_SYM(load_game,"retro_load_game");
 LOAD_SYM(unload_game,"retro_unload_game");
 LOAD_SYM(run,"retro_run");
 LOAD_SYM(get_av,"retro_get_system_av_info");
 LOAD_SYM(memory_data,"retro_get_memory_data");
 LOAD_SYM(memory_size,"retro_get_memory_size");
 LOAD_SYM(serialize_size,"retro_serialize_size");
 LOAD_SYM(serialize,"retro_serialize");
 LOAD_SYM(unserialize,"retro_unserialize");
 module_opens++;
 return 0;
fail:
 fprintf(stderr,"Gambatte missing libretro export: %s\n",dlerror());
 dlclose(lib_handle);lib_handle=NULL;memset(&api,0,sizeof(api));
 return -1;
}
static bool env(unsigned cmd, void *data){
 switch(cmd){
 case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
  if(!data)return false;
  rgb565_enabled=(*(const enum retro_pixel_format*)data==RETRO_PIXEL_FORMAT_RGB565);
  return rgb565_enabled;
 case RETRO_ENVIRONMENT_GET_CAN_DUPE:
  if(data)*(bool*)data=true;
  return true;
 case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
  if(data)*(bool*)data=false;
  return true;
 case RETRO_ENVIRONMENT_SET_VARIABLES:
 case RETRO_ENVIRONMENT_SET_CORE_OPTIONS:
 case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL:
 case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2:
 case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL:
 case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
 case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
 case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
 case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
 case RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS:
  return true;
 default:return false;
 }
}
static void present(const void *data,unsigned w,unsigned h,size_t pitch){
 if(!host_api||!host_api->present||!data||!rgb565_enabled||
    !w||!h||w>GB_MAX_W||h>GB_MAX_H||pitch<w*2u)return;
 for(unsigned y=0;y<h;y++)memcpy(frame+(size_t)y*w,(const uint8_t*)data+(size_t)y*pitch,w*2u);
 host_api->present(host_api->user,frame,w,h);
}
static size_t audio_batch(const int16_t *samples,size_t n){
 if(host_api&&host_api->audio_pcm&&samples&&n)
  host_api->audio_pcm(host_api->user,samples,n,audio_rate);
 return n;
}
static void audio_one(int16_t l,int16_t r){int16_t a[2]={l,r};audio_batch(a,1);}
static void poll(void){}
static int16_t input(unsigned port,unsigned device,unsigned index,unsigned id){
 (void)index;
 if(port||device!=RETRO_DEVICE_JOYPAD||!host_api||!host_api->buttons)return 0;
 uint32_t b=host_api->buttons(host_api->user);
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
 default:return 0;
 }
 return (b&(1u<<bit))?1:0;
}
static char *copy_str(const char *s){
 size_t n=strlen(s)+1;char *p=malloc(n);if(p)memcpy(p,s,n);return p;
}
static void restore(unsigned kind,const char *name){
 if(!host_api||!host_api->load_save)return;
 size_t n=api.memory_size(kind);
 uint8_t *p=(uint8_t*)api.memory_data(kind);
 if(!p||!n||n>(kind==RETRO_MEMORY_RTC?GB_MAX_RTC:GB_MAX_SAVE))return;
 size_t read=0;
 (void)host_api->load_save(host_api->user,name,game_path,p,n,&read);
}
static void save(unsigned kind,const char *name){
 if(!host_api||!host_api->store_save)return;
 size_t n=api.memory_size(kind);
 const uint8_t *p=(const uint8_t*)api.memory_data(kind);
 if(p&&n&&n<=(kind==RETRO_MEMORY_RTC?GB_MAX_RTC:GB_MAX_SAVE))
  (void)host_api->store_save(host_api->user,name,game_path,p,n);
}
static int start(const spmp_core_host *host,const char *path){
 if(game_loaded||!lib_handle||!host||!host->present||!host->read_entire||!path)return -1;
 /* Grow the temporary ROM buffer only when necessary. The HAL must report
  * overflow as an error; this is part of the documented read_entire contract.
  * GB 32 KiB games now avoid a wasteful 8 MiB peak allocation. */
 uint8_t *rom=NULL;
 size_t got=0,cap=32768;
 int ok=-1;
 for(;;){
  uint8_t *next=(uint8_t*)realloc(rom,cap);
  if(!next){free(rom);return -1;}
  rom=next;
  ok=host->read_entire(host->user,path,rom,cap,&got);
  if(!ok)break;
  if(cap>=GB_MAX_ROM){free(rom);return -1;}
  cap*=2;
 }
 if(got<0x150){free(rom);return -1;}
 char *name=copy_str(path);
 if(!name){free(rom);return -1;}
 host_api=host;game_path=name;
 rgb565_enabled=0;
 api.set_env(env);api.set_video(present);api.set_audio_one(audio_one);
 api.set_audio_batch(audio_batch);api.set_poll(poll);api.set_state(input);
 if(!initialized){api.init();initialized=1;}
 struct retro_game_info game={path,rom,got,NULL};
 ok=api.load_game(&game);
 free(rom);
 if(!ok||!rgb565_enabled){
  if(ok)api.unload_game();
  free(game_path);game_path=NULL;host_api=NULL;
  return -1;
 }
 struct retro_system_av_info av;
 memset(&av,0,sizeof(av));api.get_av(&av);
 if(av.timing.sample_rate>=8000&&av.timing.sample_rate<=96000)
  audio_rate=(unsigned)(av.timing.sample_rate+0.5);
 game_loaded=1;
 restore(RETRO_MEMORY_SAVE_RAM,"gb");
 restore(RETRO_MEMORY_RTC,"gb-rtc");
 return 0;
}
static int tick(void){if(!game_loaded)return -1;api.run();return 0;}
static void stop(void){
 if(!game_loaded)return;
 save(RETRO_MEMORY_SAVE_RAM,"gb");
 save(RETRO_MEMORY_RTC,"gb-rtc");
 api.unload_game();
 game_loaded=0;
 free(game_path);game_path=NULL;host_api=NULL;
}
void spmp_gambatte_module_close(void){
 if(!lib_handle)return;
 stop();
 if(initialized){api.deinit();initialized=0;}
 dlclose(lib_handle);lib_handle=NULL;
 memset(&api,0,sizeof(api));
 module_closes++;
}
void spmp_gambatte_module_stats(unsigned *opens,unsigned *closes){
 if(opens)*opens=module_opens;
 if(closes)*closes=module_closes;
}
static size_t gb_state_size(void){return game_loaded?api.serialize_size():0;}
static int gb_serialize(void *dst,size_t n){return game_loaded&&api.serialize(dst,n)?0:-1;}
static int gb_unserialize(const void *src,size_t n){return game_loaded&&api.unserialize(src,n)?0:-1;}
const spmp_core_ops spmp_gambatte_ops={start,tick,stop,gb_state_size,gb_serialize,gb_unserialize,NULL};
