#include "spmp_ui_stage10_bridge.h"
int spmp_ui_stage10_present(defos_hal *hal,uint16_t *fb,unsigned pitch,
  spmp_ui_read_at read,void*ctx,uint8_t *scratch,size_t scratch_len,
  const spmp_ui_style*style,const uint8_t*font,size_t font_len,
  const uint8_t*icons,size_t icon_len,const spmp_ui_item*items,unsigned count,
  spmp_ui_status*status,spmp_ui_state*state){
 if(!hal||!hal->video_present_rgb565||!status||!state)return -1;
 if(hal->sd_ready)status->sd_present=(uint8_t)(hal->sd_ready(hal->hardware)>0);
 if(spmp_ui_render(fb,pitch,read,ctx,scratch,scratch_len,style,font,font_len,icons,icon_len,items,count,status,state)<0)return -1;
 return hal->video_present_rgb565(hal->hardware,fb,320u,240u,pitch*2u);
}
int spmp_ui_stage10_click(defos_hal*hal,spmp_ui_sfx type,int16_t *pcm,size_t n){
 if(!hal||!hal->audio_submit_s16||!pcm||n<SPMP_UI_SOUND_FRAMES)return -1;
 size_t got=spmp_ui_sfx_generate(type,pcm,n);
 return got?hal->audio_submit_s16(hal->hardware,pcm,got,SPMP_UI_SOUND_HZ):0;
}

int spmp_ui_stage10_poll_input(defos_hal*hal,spmp_ui_state*ui,
                              uint32_t*previous_buttons,spmp_ui_sfx*sound){
 if(!hal||!hal->read_buttons||!ui||!previous_buttons||!sound)return -1;
 uint32_t held=hal->read_buttons(hal->hardware);
 uint32_t fresh=held&~*previous_buttons;
 *previous_buttons=held;
 spmp_ui_key key=(spmp_ui_key)0;
 if(fresh&DEFOS_KEY_SELECT)key=SPMP_UI_TOGGLE_LAYOUT;
 else if(fresh&DEFOS_KEY_A)key=SPMP_UI_ACCEPT;
 else if(fresh&DEFOS_KEY_B)key=SPMP_UI_BACK;
 else if(fresh&DEFOS_KEY_UP)key=SPMP_UI_UP;
 else if(fresh&DEFOS_KEY_DOWN)key=SPMP_UI_DOWN;
 else if(fresh&DEFOS_KEY_LEFT)key=SPMP_UI_LEFT;
 else if(fresh&DEFOS_KEY_RIGHT)key=SPMP_UI_RIGHT;
 *sound=key?spmp_ui_key_event(ui,key):SPMP_UI_SFX_NONE;
 return key?1:0;
}
