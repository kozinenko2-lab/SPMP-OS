#include "spmp_cores.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
extern const spmp_core_ops spmp_sms_ops;
extern unsigned spmp_sms_debug_ram0(void);
typedef struct{unsigned video,audio;} stats;
static void present(void *ctx,const uint16_t *rgb,unsigned w,unsigned h){
 stats*s=ctx;assert(rgb&&w>=160&&w<=256&&h>=144&&h<=240);s->video++;
}
static void audio(void *ctx,const int16_t *pcm,size_t frames,unsigned rate){
 stats*s=ctx;assert(pcm&&frames&&rate==32000);s->audio++;
}
static int readrom(void *ctx,const char *name,uint8_t *dst,size_t cap,size_t *n){
 (void)ctx;(void)name;if(cap<32768)return -1;
 memset(dst,0,32768);dst[0]=0x3e;dst[1]=0x5a;
 dst[2]=0x32;dst[3]=0x00;dst[4]=0xc0;dst[5]=0x18;dst[6]=0xfe;
 memcpy(dst+0x7ff0,"TMR SEGA",8);dst[0x7fff]=0x40;*n=32768;
 return 0;
}
int main(void){
 stats s={0};spmp_core_host host={0};host.user=&s;host.present=present;host.audio_pcm=audio;host.read_entire=readrom;
 for(unsigned cycle=0;cycle<12;cycle++){
  assert(spmp_sms_ops.start(&host,"test.sms")==0);
  for(unsigned f=0;f<10;f++)assert(spmp_sms_ops.tick()==0);
  assert(spmp_sms_debug_ram0()==0x5a);
  spmp_sms_ops.stop();
 }
 assert(s.video==120&&s.audio>=120);
 puts("SMSPlusGX ASAN: 120 real CPU+VDP+PSG frames, 12 open/close cycles OK");
 return 0;
}
