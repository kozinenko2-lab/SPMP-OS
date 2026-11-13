/* Actual Snes9x libretro core: synthetic homebrew LoROM smoke test. */
#include "spmp_cores.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct {unsigned frames,samples,last_w,last_h;} ctx;
static void present(void *u,const uint16_t *pix,unsigned w,unsigned h){ctx*c=u;assert(pix&&w>=256&&w<=512&&h>=200&&h<=512);c->frames++;c->last_w=w;c->last_h=h;}
static uint32_t buttons(void *u){(void)u;return 0;}
static void audio(void *u,const int16_t *p,size_t count,unsigned rate){ctx*c=u;assert(p&&rate>=8000&&rate<=96000);c->samples+=(unsigned)count;}
static int rom(void*u,const char *path,uint8_t *dst,size_t max,size_t *n){
 (void)u;(void)path;if(max<32768)return -1;
 memset(dst,0xff,32768);
 /* Reset vector 0x8000 maps to LoROM bank 00, ROM offset zero. */
 dst[0]=0x78;dst[1]=0x80;dst[2]=0xfe; /* SEI; BRA -2 */
 memcpy(dst+0x7fc0,"SPMP-OS TEST CART",17);
 dst[0x7fd5]=0x20;dst[0x7fd6]=0;dst[0x7fd7]=5;dst[0x7fd8]=0;
 dst[0x7fd9]=1;dst[0x7fda]=0x33;
 dst[0x7fdc]=0xff;dst[0x7fdd]=0xff;dst[0x7fde]=0;dst[0x7fdf]=0;
 dst[0x7ffc]=0x00;dst[0x7ffd]=0x80;
 *n=32768;return 0;
}
static int size_of_rom(void *u,const char *p,size_t *n){
 (void)u;if(strstr(p,"test.md"))return -1;*n=32768;return 0;
}
int main(void){
 ctx c={0};spmp_core_host h={0};h.user=&c;h.present=present;h.buttons=buttons;h.audio_pcm=audio;h.read_entire=rom;h.file_size=size_of_rom;
 for(unsigned k=0;k<2;k++){
  assert(!spmp_core_launch("snes",&h,k?"/roms/two.sfc":"/roms/one.sfc"));
  for(unsigned i=0;i<5;i++)assert(spmp_core_tick()==0);
  assert(c.frames>0);
  assert(!spmp_core_pause_to_menu());assert(spmp_core_tick()==2);
  /* Missing Mega Drive cartridge must not discard paused SNES. */
  assert(spmp_core_launch("megadrive",&h,"/roms/test.md") != 0);
  assert(spmp_core_is_paused()==1);assert(spmp_core_tick()==2);
  size_t bytes=spmp_core_state_size();assert(bytes>0&&bytes<8u*1024u*1024u);
  uint8_t *state=malloc(bytes);assert(state);
  assert(!spmp_core_serialize(state,bytes));assert(!spmp_core_unserialize(state,bytes));free(state);
  assert(!spmp_core_resume());
  spmp_core_stop();
  assert(spmp_core_cached_id()&&!strcmp(spmp_core_cached_id(),"snes"));
 }
 assert(c.frames>=10);
 spmp_core_shutdown();
 printf("SNES actual core: %u frames, %u audio frames, video %ux%u, save/load passed\n",c.frames,c.samples,c.last_w,c.last_h);
 return 0;
}
