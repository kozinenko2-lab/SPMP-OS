#include "spmp_cores.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
typedef struct{unsigned frames,audio_frames,w,h; uint16_t first;} ctx;
static void video(void*u,const uint16_t*p,unsigned w,unsigned h){ctx*c=u;assert(p&&w==240&&h==160);c->frames++;c->w=w;c->h=h;c->first=p[0];}
static uint32_t buttons(void*u){(void)u;return 0;}
static void audio(void*u,const int16_t*p,size_t n,unsigned rate){ctx*c=u;assert(p&&rate>=8000&&rate<=96000);c->audio_frames+=(unsigned)n;}
static int size(void*u,const char*p,size_t*n){(void)u;struct stat s;if(stat(p,&s)||s.st_size<=0)return -1;*n=(size_t)s.st_size;return 0;}
static int readall(void*u,const char*p,uint8_t*d,size_t cap,size_t*n){(void)u;FILE*f=fopen(p,"rb");if(!f)return -1;*n=fread(d,1,cap,f);int extra=fgetc(f),err=ferror(f);fclose(f);return (extra==EOF&&!err)?0:-1;}
int main(int argc,char**argv){assert(argc==2);ctx c={0};spmp_core_host h={0};h.user=&c;h.present=video;h.buttons=buttons;h.audio_pcm=audio;h.file_size=size;h.read_entire=readall;
const spmp_core_desc *d=spmp_core_find("gba");assert(d&&d->ops&&d->state==SPMP_CORE_HOST_TESTED);
assert(spmp_core_launch("gba",&h,argv[1])==0);
for(unsigned i=0;i<12;i++)assert(spmp_core_tick()==0);
assert(c.frames>=10);assert(c.audio_frames>0);assert(c.first==0xf800 && "ARM7 VRAM write must reach RGB565 output");
assert(spmp_core_pause_to_menu()==0);assert(spmp_core_tick()==2);
size_t sz=spmp_core_state_size();assert(sz>0 && sz<8u*1024u*1024u);
void *state=malloc(sz);assert(state);
assert(spmp_core_serialize(state,sz)==0);assert(spmp_core_unserialize(state,sz)==0);free(state);
assert(spmp_core_resume()==0);assert(spmp_core_tick()==0);assert(c.first==0xf800);
spmp_core_stop();assert(spmp_core_cached_id() && !strcmp(spmp_core_cached_id(),"gba"));
assert(spmp_core_launch("gba",&h,argv[1])==0);for(int i=0;i<3;i++)assert(spmp_core_tick()==0);assert(c.first==0xf800);
spmp_core_shutdown();printf("gpSP: frames=%u audio=%u size=%ux%u pixel=0x%04x saves + reload OK\n",c.frames,c.audio_frames,c.w,c.h,c.first);return 0;
}
