#define _POSIX_C_SOURCE 200809L
#include "spmp_game_menu.h"
#include "spmp_ui_posix.h"
#include "spmp_ui_router.h"
#include "spmp_cores.h"
#include "spmp_theme.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned frames;
static void preview_dump(const char *id, const spmp_game_menu *m){
 uint16_t *img=malloc(320u*240u*2u);assert(img);
 spmp_theme theme;spmp_theme_default(&theme);
 spmp_menu_draw(m,&theme,img,320,240);
 char name[128];snprintf(name,sizeof name,"build/stage15-%s-menu.ppm",id);
 FILE *f=fopen(name,"wb");assert(f);
 fprintf(f,"P6\n320 240\n255\n");
 for(size_t i=0;i<320u*240u;i++){
  uint16_t px=img[i];
  uint8_t rgb[3]={(uint8_t)(((px>>11)&31u)*255u/31u),
   (uint8_t)(((px>>5)&63u)*255u/63u),(uint8_t)((px&31u)*255u/31u)};
  assert(fwrite(rgb,1,3,f)==3);
 }
 assert(!fclose(f));free(img);
}

static uint32_t buttons(void *ctx){(void)ctx;return 0;}
static void present(void *ctx,const uint16_t *img,unsigned w,unsigned h){
 (void)ctx;assert(img&&w&&h);frames++;
}
static int read_all(void *ctx,const char*path,uint8_t*dst,size_t cap,size_t*got){
 (void)ctx;FILE*f=fopen(path,"rb");if(!f)return -1;
 size_t n=fread(dst,1,cap,f);
 int over=fgetc(f)!=EOF,err=ferror(f);fclose(f);
 if(over||err)return -1;
 *got=n;
 return 0;
}
static void audio(void *ctx,const int16_t *pcm,size_t n,unsigned hz){
 (void)ctx;assert(pcm&&n&&hz>=8000&&hz<=96000);
}
static int load_battery(void *ctx,const char*core,const char*path,uint8_t *dst,size_t cap,size_t *got){
 (void)ctx;(void)core;(void)path;(void)dst;(void)cap;*got=0;return -1;
}
static int store_battery(void *ctx,const char*core,const char*path,const uint8_t*dst,size_t n){
 (void)ctx;(void)core;(void)path;(void)dst;(void)n;return 0;
}
static int check_one(const char *id,const char *rom,
 const spmp_core_host *base,const spmp_ui_storage *storage,uint8_t *scratch){
 spmp_ui_emulator emu=spmp_game_core_emulator();
 spmp_game_menu *m=malloc(sizeof(*m));if(!m)return -1;
 spmp_menu_init(m,id,rom,storage,&emu,scratch,3u*1024u*1024u);
 spmp_ui_router router;
 assert(!spmp_ui_wrap_host(&router,m,base));
 if(spmp_core_launch(id,&router.wrapped,rom)){free(m);return -2;}
 for(int i=0;i<3;i++)if(spmp_core_tick()){free(m);return -3;}
 assert(m->clean_valid && frames>0);
 assert(!spmp_menu_input(m,SPMP_UI_POWER,1000));
 assert(spmp_menu_input(m,0,1180)==1 && m->opened);
 assert(spmp_core_is_paused());assert(spmp_core_tick()==2);
 size_t n=spmp_core_state_size();assert(n>0&&n<2u*1024u*1024u);
 assert(!spmp_menu_save(m));
 assert(m->preview_valid);
 preview_dump(id,m);
 uint16_t pixel=m->clean_frame[550];assert(pixel==m->preview[550]);
 /* A changed emulator state must still successfully restore from disk. */
 assert(!spmp_menu_input(m,SPMP_UI_POWER,1300));
 assert(spmp_menu_input(m,0,1480)==1);
 assert(!spmp_core_is_paused());
 for(int i=0;i<4;i++)assert(!spmp_core_tick());
 assert(!spmp_menu_input(m,SPMP_UI_POWER,1600));
 assert(spmp_menu_input(m,0,1780)==1);
 assert(!spmp_menu_load(m));
 assert(m->preview[550]==pixel);
 /* Exit game but keep code resident for the next ROM on same core. */
 assert(!emu.exit_to_library(emu.user));
 assert(!spmp_core_is_paused());
 free(m);return 0;
}
int main(void){
 char dir[]="/tmp/spmp-cores-XXXXXX";assert(mkdtemp(dir));
 spmp_ui_posix fs;assert(!spmp_ui_posix_init(&fs,dir));
 spmp_ui_storage storage=spmp_ui_posix_storage(&fs);
 uint8_t *scratch=malloc(3u*1024u*1024u);assert(scratch);
 spmp_core_host host={0};host.present=present;host.buttons=buttons;
 host.read_entire=read_all;host.audio_pcm=audio;host.load_save=load_battery;host.store_save=store_battery;
 assert(!check_one("gb","samples/spmp_battery.gb",&host,&storage,scratch));
 assert(!check_one("nes","samples/nes_cpu_ppu_smoke.nes",&host,&storage,scratch));
 assert(!strcmp(spmp_core_cached_id(),"nes"));
 spmp_core_shutdown();free(scratch);
 printf("SPMP-OS live cores: Gambatte + FCEUmm, real serialized states, RGB565 thumbnail, common pause/restore OK\n");
 return 0;
}
