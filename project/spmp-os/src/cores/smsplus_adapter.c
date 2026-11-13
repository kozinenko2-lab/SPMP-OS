/* SMS Plus GX / Retro-Go core adapter to SPMP-OS. GPL-2.0-or-later.
   Kept outside firmware until HAL and ARMv5 relocation tests complete. */
#include "spmp_cores.h"
#include "../../third_party/smsplus/shared.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_ROM (2u * 1024u * 1024u)
#define WIDTH 256u
#define HEIGHT 240u
static const spmp_core_host *host;
static uint8_t *indexed;
static uint16_t *screen;
static int16_t pcm[2u*1024u];
static int active;
static unsigned frames;
static unsigned audio_frames;
static char game_path[384];

static int endswith(const char *s,const char *suffix){
 if(!s||!suffix)return 0;
 size_t n=strlen(s),m=strlen(suffix);if(n<m)return 0;
 s+=n-m;
 for(size_t i=0;i<m;i++){
  char a=s[i],b=suffix[i];
  if(a>='A'&&a<='Z')a+=32;
  if(b>='A'&&b<='Z')b+=32;
  if(a!=b)return 0;
 }
 return 1;
}
static void release(void){
 if(active){system_shutdown();active=0;}
 spmp_sms_unload_image();
 free(indexed);indexed=NULL;
 free(screen);screen=NULL;
 host=NULL;
}
static int start(const spmp_core_host *h,const char *path){
 if(active||!h||!h->read_entire||!h->present||!path)return -1;
 host=h;
 indexed=(uint8_t*)calloc(WIDTH*HEIGHT,1);
 screen=(uint16_t*)calloc(WIDTH*HEIGHT,sizeof(uint16_t));
 uint8_t *raw=(uint8_t*)malloc(MAX_ROM+512u);
 if(!indexed||!screen||!raw){free(raw);release();return -1;}
 size_t n=0;
 int err=h->read_entire(h->user,path,raw,MAX_ROM+512u,&n);
 if(err||!n||n>MAX_ROM+512u){free(raw);release();return -1;}
 system_reset_config();
 int gg=endswith(path,".gg");
 if(spmp_sms_load_image(raw,n,gg)){free(raw);release();return -1;}
 free(raw);
 bitmap.data=indexed;bitmap.width=WIDTH;bitmap.height=HEIGHT;
 bitmap.pitch=WIDTH;bitmap.granularity=0;
 option.sndrate=32000;option.overscan=0;option.extra_gg=0;
 system_init2();system_reset();active=1;
 frames=0;audio_frames=0;
 size_t pathlen=strlen(path);if(pathlen>=sizeof game_path){release();return -1;}
 memcpy(game_path,path,pathlen+1);
 if(h->load_save&&cart.sram){size_t got=0;
  if(!h->load_save(h->user,"sms",game_path,cart.sram,0x8000u,&got)){
   if(got>0&&got<=0x8000u)sms.save=1;
  }
 }
 return 0;
}
static int tick(void){
 if(!active||!host)return -1;
 uint32_t b=host->buttons?host->buttons(host->user):0;
 input.pad[0]=0;input.pad[1]=0;input.system=0;
 if(b&1u)input.pad[0]|=INPUT_UP;
 if(b&2u)input.pad[0]|=INPUT_DOWN;
 if(b&4u)input.pad[0]|=INPUT_LEFT;
 if(b&8u)input.pad[0]|=INPUT_RIGHT;
 if(b&(1u<<4))input.pad[0]|=INPUT_BUTTON1;
 if(b&(1u<<5))input.pad[0]|=INPUT_BUTTON2;
 if(b&(1u<<6))input.system|=(sms.console==CONSOLE_GG)?INPUT_START:INPUT_PAUSE;
 system_frame(0);
 int w=bitmap.viewport.w,h=bitmap.viewport.h;
 int x=bitmap.viewport.x,y=bitmap.viewport.y;
 if(w<=0||h<=0||w>(int)WIDTH||h>(int)HEIGHT||x<0||x+w>(int)WIDTH||y<0||y+h>(int)HEIGHT)return -1;
 uint16 pal[32];render_copy_palette(pal);
 for(int r=0;r<h;r++)for(int c=0;c<w;c++){
  uint8_t pixel=indexed[(r+y)*WIDTH+c+x];
  uint16 color=pal[pixel&31u];
  screen[r*w+c]=(uint16_t)((color>>8)|(color<<8));
 }
 host->present(host->user,screen,(unsigned)w,(unsigned)h);
 if(host->audio_pcm&&snd.enabled&&snd.output[0]&&snd.output[1]&&snd.sample_count>0&&snd.sample_count<=1024){
  for(int i=0;i<snd.sample_count;i++){
   pcm[2u*(unsigned)i]=snd.output[0][i];
   pcm[2u*(unsigned)i+1u]=snd.output[1][i];
  }
  host->audio_pcm(host->user,pcm,(size_t)snd.sample_count,(unsigned)snd.sample_rate);
  audio_frames+=(unsigned)snd.sample_count;
 }
 frames++;
 return 0;
}
static void stop(void){
 if(active&&sms.save&&host&&host->store_save&&cart.sram){
  (void)host->store_save(host->user,"sms",game_path,cart.sram,0x8000u);
 }
 release();
}
/* Real adapter supports deferred region (restart core/ROM to apply).
 * Other settings are handled at the shared audio/input router. */
static int option_set(unsigned id,int value){
 (void)value;
 if(id==5u||id==8u)return 1;
 return 1;
}
__attribute__((visibility("default"))) const spmp_core_ops spmp_sms_ops={start,tick,stop,NULL,NULL,NULL,option_set};
__attribute__((visibility("default"))) unsigned spmp_sms_debug_frames(void){return frames;}
__attribute__((visibility("default"))) unsigned spmp_sms_debug_ram0(void){return sms.wram[0];}
unsigned spmp_sms_debug_audio_frames(void){return audio_frames;}
