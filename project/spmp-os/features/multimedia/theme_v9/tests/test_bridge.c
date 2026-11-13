#include "spmp_ui_stage10_bridge.h"
#include "spmp_ui_builtin_glyphs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint32_t buttons;
static unsigned presentations,played;
static uint32_t readbtn(void*ctx){(void)ctx;return buttons;}
static int sd(void*ctx){(void)ctx;return 0;}
static int video(void*ctx,const uint16_t*fb,unsigned w,unsigned h,unsigned pitch){(void)ctx;assert(fb);assert(w==320&&h==240&&pitch==640);presentations++;return 0;}
static int audio(void*ctx,const int16_t*pcm,size_t n,unsigned hz){(void)ctx;assert(pcm&&n==640&&hz==16000);played++;return 0;}
int main(void){
 defos_hal hal={readbtn,video,audio,sd,0,0};spmp_ui_state u;spmp_ui_sfx sound;uint32_t prior=0;
 spmp_ui_state_init(&u,SPMP_UI_LAYOUT_LIST,8);
 buttons=DEFOS_KEY_DOWN;assert(spmp_ui_stage10_poll_input(&hal,&u,&prior,&sound)==1&&u.selected==1&&sound==SPMP_UI_SFX_MOVE);
 assert(spmp_ui_stage10_poll_input(&hal,&u,&prior,&sound)==0&&u.selected==1);
 buttons=0;assert(spmp_ui_stage10_poll_input(&hal,&u,&prior,&sound)==0);
 buttons=DEFOS_KEY_SELECT;assert(spmp_ui_stage10_poll_input(&hal,&u,&prior,&sound)==1&&u.layout==SPMP_UI_LAYOUT_GRID);
 spmp_ui_item items[]={{"Игры",0},{"Музыка",1}};
 spmp_ui_status st={255,255,50,1};spmp_ui_style style={0xf800,0x0000,0xffff,0x8888,0x6000,0x2222,0,1};
 unsigned char icon[8+8*54]={ 'S','P','I','1',8,0,18,18};
 unsigned char scratch[2560];uint16_t fb[320*240];size_t flen=0;
 const uint8_t*font=spmp_ui_builtin_glyphs(&flen);
 assert(spmp_ui_stage10_present(&hal,fb,320,0,0,scratch,sizeof scratch,&style,font,flen,icon,sizeof icon,items,2,&st,&u)==0);
 assert(st.sd_present==0&&presentations==1); /* missing SD fallback must still present */
 int16_t pcm[SPMP_UI_SOUND_FRAMES*2];assert(spmp_ui_stage10_click(&hal,SPMP_UI_SFX_ACCEPT,pcm,SPMP_UI_SOUND_FRAMES)==0&&played==1);
 puts("PASS Stage10 input mapping, absent SD fallback, LCD and UI audio hooks");
 return 0;
}
