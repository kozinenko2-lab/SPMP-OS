#include "spmp_cores.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern unsigned spmp_sms_debug_ram0(void);
typedef struct {unsigned frames,audio,width,height;} ctx;
static void present(void *u,const uint16_t *pixels,unsigned w,unsigned h){
 ctx*c=u;assert(pixels&&w>=160&&w<=256&&h>=144&&h<=240);c->frames++;c->width=w;c->height=h;
}
static uint32_t buttons(void*u){(void)u;return 0;}
static void audio(void*u,const int16_t*data,size_t frames,unsigned hz){
 ctx*c=u;assert(data&&frames&&hz==32000);c->audio+=(unsigned)frames;
}
static int readrom(void*u,const char*path,uint8_t*dst,size_t cap,size_t*n){
 (void)u;if(cap<0x8000)return -1;memset(dst,0,0x8000);
 /* Z80: LD A,5A ; LD (C000),A ; JR -2 */
 dst[0]=0x3e;dst[1]=0x5a;
 dst[2]=0x32;dst[3]=0x00;dst[4]=0xc0;
 dst[5]=0x18;dst[6]=0xfe;
 memcpy(dst+0x7ff0,"TMR SEGA",8);
 dst[0x7fff]=(strstr(path,".gg")?0x60:0x40);
 *n=0x8000;return 0;
}
static int run(const char*path,unsigned expected_w){
 ctx c={0};spmp_core_host h={0};h.user=&c;h.present=present;h.buttons=buttons;h.audio_pcm=audio;h.read_entire=readrom;
 assert(!spmp_core_launch("sms",&h,path));
 for(int i=0;i<5;i++)assert(spmp_core_tick()==0);
 assert(c.frames==5&&c.audio>0);
 assert(c.width==expected_w);
 assert(spmp_sms_debug_ram0()==0x5a);
 assert(!spmp_core_pause_to_menu());assert(spmp_core_tick()==2);
 assert(!spmp_core_resume());assert(!spmp_core_tick());
 spmp_core_stop();
 assert(spmp_core_cached_id() && !strcmp(spmp_core_cached_id(),"sms"));
 return 0;
}
int main(void){
 assert(!run("/roms/a.sms",256));
 assert(!run("/roms/b.gg",160));
 /* Switching platform must unload the dynamic SMS shared module. */
 ctx other={0};spmp_core_host h={0};h.user=&other;h.present=present;h.buttons=buttons;h.read_entire=readrom;
 assert(!spmp_core_launch("diagnostic",&h,NULL));
 assert(!strcmp(spmp_core_cached_id(),"diagnostic"));
 spmp_core_shutdown();
 puts("SMS Plus GX: actual Z80 frames, VDP video, SN76489 PCM, stop/restart OK");
 return 0;
}
