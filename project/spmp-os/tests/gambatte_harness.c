/* SPMP-OS Gambatte: original ROM code, RGB565, audio, SRAM and true module
 * residency tests across two games and a different core. */
#include "spmp_cores.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

extern void spmp_gambatte_module_stats(unsigned*,unsigned*);
static struct {
 unsigned frames,audio_calls,bad,save_calls;
 uint8_t first_save[8192],second_save[8192];
 size_t first_len,second_len;
} state;
static void present(void*u,const uint16_t *pixels,unsigned w,unsigned h){
 (void)u;
 if(!pixels||!(w==160&&h==144))state.bad++;
 else state.frames++;
}
static uint32_t buttons(void*u){(void)u;return 0;}
static int read_all(void*u,const char *path,uint8_t *dst,size_t cap,size_t *len){
 (void)u;
 FILE*f=fopen(path,"rb");if(!f)return -1;
 size_t n=fread(dst,1,cap,f);
 int overflow=fgetc(f)!=EOF,fail=ferror(f);
 fclose(f);
 if(overflow||fail){return -1;}
 *len=n;return 0;
}
static void audio(void*u,const int16_t *pcm,size_t n,unsigned rate){
 (void)u;
 if(!pcm||!n||rate<8000||rate>96000)state.bad++;
 else state.audio_calls++;
}
static int store_save(void *u,const char *core,const char *game,const uint8_t *bytes,size_t n){
 (void)u;
 if(strcmp(core,"gb"))return 0; /* RTC handled independently */
 uint8_t *dst=strstr(game,"battery")?state.first_save:state.second_save;
 size_t *len=strstr(game,"battery")?&state.first_len:&state.second_len;
 if(n>8192)return -1;
 memcpy(dst,bytes,n);*len=n;state.save_calls++;
 return 0;
}
static int load_save(void *u,const char *core,const char *game,uint8_t *dst,size_t cap,size_t *loaded){
 (void)u;
 if(strcmp(core,"gb"))return -1;
 uint8_t *src=strstr(game,"battery")?state.first_save:state.second_save;
 size_t n=strstr(game,"battery")?state.first_len:state.second_len;
 if(n>cap)return -1;
 if(n)memcpy(dst,src,n);
 *loaded=n;return 0;
}
static int check_count(unsigned opens,unsigned closes){
 unsigned a=0,b=0;spmp_gambatte_module_stats(&a,&b);
 if(a!=opens||b!=closes){fprintf(stderr,"module counters %u/%u expected %u/%u\n",a,b,opens,closes);return -1;}
 return 0;
}
int main(void){
 spmp_core_host host={0};
 host.present=present;host.buttons=buttons;host.read_entire=read_all;
 host.audio_pcm=audio;host.store_save=store_save;host.load_save=load_save;
 if(spmp_core_launch("gb",&host,"samples/spmp_battery.gb"))return 10;
 if(check_count(1,0)||strcmp(spmp_core_cached_id(),"gb"))return 11;
 for(int i=0;i<6;i++)if(spmp_core_tick())return 12;
 if(state.frames!=6||!state.audio_calls||state.bad)return 13;
 /* Common savestate callbacks: only callable through the unified menu while paused. */
 if(spmp_core_state_size()==0)return 33;
 size_t stsize=spmp_core_state_size();
 if(stsize>2u*1024u*1024u)return 34;
 uint8_t *saved=malloc(stsize);if(!saved)return 35;
 if(spmp_core_serialize(saved,stsize)==0)return 36; /* must be paused */
 if(spmp_core_pause_to_menu())return 37;
 if(spmp_core_serialize(saved,stsize))return 38;
 if(spmp_core_unserialize(saved,stsize))return 39;
 free(saved);
 if(spmp_core_resume())return 40;
 /* Menu does not unload the running ROM or the current emulator. */
 if(spmp_core_pause_to_menu()||!spmp_core_is_paused())return 28;
 if(spmp_core_tick()!=2||state.frames!=6)return 29;
 if(spmp_core_launch("gb",&host,"samples/spmp_battery.gb"))return 30;
 if(spmp_core_is_paused()||check_count(1,0))return 31;
 if(spmp_core_tick()||state.frames!=7)return 32;
 /* A second ROM on Gambatte must not dlclose/dlopen the module. */
 if(spmp_core_launch("gb",&host,"samples/spmp_color.gbc"))return 14;
 if(check_count(1,0)||strcmp(spmp_core_cached_id(),"gb"))return 15;
 if(state.first_len!=8192||state.first_save[0]!=0x42)return 16;
 for(int i=0;i<5;i++)if(spmp_core_tick())return 17;
 if(state.frames!=12||state.bad)return 18;
 /* Exiting the game frees its ROM but retains the emulator. */
 spmp_core_stop();
 if(check_count(1,0)||strcmp(spmp_core_cached_id(),"gb"))return 19;
 if(state.second_len!=8192||state.second_save[0]!=0x57)return 20;
 /* Loading another core really closes the Gambatte .so on the host. */
 if(spmp_core_launch("diagnostic",&host,NULL))return 21;
 if(check_count(1,1)||strcmp(spmp_core_cached_id(),"diagnostic"))return 22;
 if(spmp_core_tick())return 23;
 /* Switching back to Gambatte loads it a second time; old save restored. */
 if(spmp_core_launch("gb",&host,"samples/spmp_battery.gb"))return 24;
 if(check_count(2,1))return 25;
 for(int i=0;i<3;i++)if(spmp_core_tick())return 26;
 spmp_core_shutdown();
 if(check_count(2,2)||spmp_core_cached_id())return 27;
 printf("Gambatte GB/GBC: %u frames, %u audio batches, SRAM/RTC callbacks, module opens=2 closes=2 OK\n",state.frames,state.audio_calls);
 return 0;
}
