#include "spmp_hw_adapter.h"
#include <string.h>
int spmp_legacy_input_init(spmp_legacy_input *in,spmp_legacy_key_init init,
 spmp_legacy_key_get read,spmp_legacy_key_cleanup close){
 if(!in||!init||!read)return -1;
 memset(in,0,sizeof(*in));in->legacy.controller=0;
 if(init(&in->legacy))return -1;
 for(unsigned i=0;i<20;i++)in->mapping.raw[i]=in->legacy.scancode[i];
 in->read=read;in->cleanup=close;in->ready=1;return 0;
}
uint32_t spmp_legacy_raw(spmp_legacy_input *in){
 return in&&in->ready&&in->read?in->read(&in->legacy):0;
}
uint32_t spmp_legacy_buttons(spmp_legacy_input *in){
 return in&&in->ready?spmp_hw_translate(&in->mapping,spmp_legacy_raw(in)):0;
}
void spmp_legacy_input_close(spmp_legacy_input *in){
 if(!in||!in->ready)return;
 if(in->cleanup)in->cleanup(&in->legacy);
 in->ready=0;
}
void spmp_legacy_confirm_power(spmp_legacy_input *in,uint32_t mask,unsigned low){
 if(!in||!in->ready)return;
 /* Unknown 0 or overlapping mapped button is not a reliable power key. */
 uint32_t used=0;
 for(unsigned i=0;i<20;i++)used|=in->legacy.scancode[i];
 if(mask&&(mask&(mask-1u))==0&&!(mask&used)){
  in->mapping.power_raw_mask=mask;in->mapping.power_active_low=(uint8_t)!!low;
 }
}
int spmp_hw_adapter_ready(const spmp_hw_adapter *h){
 return h&&h->hardware_verified&&h->read_raw&&h->clock_ms&&h->present&&
        h->validated_ram_start&&h->validated_ram_size;
}
uint32_t spmp_hw_buttons(const spmp_hw_adapter *h){
 return h&&h->read_raw?spmp_hw_translate(&h->keys,h->read_raw(h->user)):0;
}
uint32_t spmp_hw_milliseconds(const spmp_hw_adapter *h){
 return h&&h->clock_ms?h->clock_ms(h->user):0;
}
int spmp_hw_present(const spmp_hw_adapter *h,const uint16_t *rgb,
 unsigned w,unsigned height,unsigned pitch){
 if(!h||!h->present||!rgb||!w||!height||w>4096||height>4096||pitch<w*2u)return -1;
 return h->present(h->user,rgb,w,height,pitch);
}
