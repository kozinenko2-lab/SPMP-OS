#include "spmp_input_hal.h"
uint32_t spmp_hw_translate(const spmp_hw_keymap *m,uint32_t raw){
 if(!m)return 0;
 uint32_t b=0;
 static const unsigned mask[]={SPMP_UI_UP,SPMP_UI_DOWN,SPMP_UI_LEFT,SPMP_UI_RIGHT};
 for(unsigned i=0;i<4;i++)if(m->raw[i]&&(raw&m->raw[i]))b|=mask[i];
 if(m->raw[SPMP_HW_O]&&(raw&m->raw[SPMP_HW_O]))b|=SPMP_UI_CONFIRM;
 if(m->raw[SPMP_HW_X]&&(raw&m->raw[SPMP_HW_X]))b|=SPMP_UI_BACK;
 if(m->power_raw_mask){
  unsigned pressed=m->power_active_low?!(raw&m->power_raw_mask):!!(raw&m->power_raw_mask);
  if(pressed)b|=SPMP_UI_POWER;
 }
 return b;
}
uint32_t spmp_hw_unmapped(const spmp_hw_keymap *m,uint32_t raw){
 if(!m)return raw;
 uint32_t known=m->power_raw_mask;
 for(unsigned i=0;i<20;i++)known|=m->raw[i];
 return raw&~known;
}
