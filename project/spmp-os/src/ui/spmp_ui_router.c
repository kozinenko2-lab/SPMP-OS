#include "spmp_ui_router.h"
#include <string.h>
static void present(void *ctx,const uint16_t *pixels,unsigned w,unsigned h){
 spmp_ui_router *r=(spmp_ui_router*)ctx;
 if(r->menu&&!r->menu->opened)
  (void)spmp_menu_capture_clean(r->menu,pixels,w,h,(size_t)w*2u);
 if(r->underlying.present)r->underlying.present(r->underlying.user,pixels,w,h);
}
static uint32_t buttons(void *ctx){
 spmp_ui_router *r=(spmp_ui_router*)ctx;
 if(r->menu&&r->menu->opened)return 0;
 uint32_t keys=r->underlying.buttons?r->underlying.buttons(r->underlying.user):0;
 if(r->menu&&spmp_settings_get(&r->menu->settings,SPMP_OPT_SWAP_AB,0)){
  uint32_t a=keys&(1u<<4),b=keys&(1u<<5);
  keys=(keys&~((1u<<4)|(1u<<5)))|(a<<1)|(b>>1);
 }
 return keys;
}
static int read_entire(void *ctx,const char *path,uint8_t *dst,size_t cap,size_t *len){
 spmp_ui_router*r=ctx;
 return r->underlying.read_entire?r->underlying.read_entire(r->underlying.user,path,dst,cap,len):-1;
}
static void audio(void *ctx,const int16_t *src,size_t n,unsigned hz){
 spmp_ui_router*r=ctx;
 if(r->menu&&r->menu->opened)return;
 if(!r->underlying.audio_pcm||!src)return;
 int volume=r->menu?spmp_settings_get(&r->menu->settings,SPMP_OPT_VOLUME,10):10;
 if(volume>=10){r->underlying.audio_pcm(r->underlying.user,src,n,hz);return;}
 /* Bounded scratch avoids allocating frames in realtime callback. */
 int16_t tmp[2u*256u];size_t offset=0;
 while(offset<n){
  size_t frames=n-offset;if(frames>256)frames=256;
  for(size_t i=0;i<frames*2u;i++){
   int32_t v=((int32_t)src[offset*2u+i]*volume)/10;
   tmp[i]=(int16_t)v;
  }
  r->underlying.audio_pcm(r->underlying.user,tmp,frames,hz);
  offset+=frames;
 }
}
static int load_save(void *ctx,const char *core,const char *path,uint8_t*dst,size_t cap,size_t *n){
 spmp_ui_router*r=ctx;
 return r->underlying.load_save?r->underlying.load_save(r->underlying.user,core,path,dst,cap,n):-1;
}
static int store_save(void *ctx,const char *core,const char *path,const uint8_t *src,size_t n){
 spmp_ui_router*r=ctx;
 return r->underlying.store_save?r->underlying.store_save(r->underlying.user,core,path,src,n):-1;
}
static int file_size(void *ctx,const char *path,size_t *bytes){
 spmp_ui_router *r=ctx;
 return r->underlying.file_size?r->underlying.file_size(r->underlying.user,path,bytes):-1;
}
int spmp_ui_wrap_host(spmp_ui_router *r,spmp_game_menu *m,const spmp_core_host *source){
 if(!r||!source||!source->present||!source->read_entire)return -1;
 memset(r,0,sizeof *r);r->menu=m;r->underlying=*source;
 r->wrapped.user=r;r->wrapped.present=present;r->wrapped.buttons=buttons;
 r->wrapped.read_entire=read_entire;r->wrapped.audio_pcm=audio;
 r->wrapped.load_save=load_save;r->wrapped.store_save=store_save;
 if(source->file_size)r->wrapped.file_size=file_size;
 return 0;
}
