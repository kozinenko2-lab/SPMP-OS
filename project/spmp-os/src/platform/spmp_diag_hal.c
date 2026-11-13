#include "spmp_diag_hal.h"
#include <string.h>
int spmp_diag_readonly_snapshot(const spmp_diag_hal *h,spmp_diag_snapshot *dst){
    if(!h||!dst||!h->confirm_readonly_api||h->confirm_readonly_api(h->context)!=1)return -1;
    spmp_diag_snapshot s;memset(&s,0,sizeof(s));
    uint32_t k=0;
    if(h->read_keys){if(h->read_keys(h->context,&k)==0){s.keys=k;s.available|=SPMP_DIAG_KEYS;}
                     else s.failed|=SPMP_DIAG_KEYS;}
    uint32_t ms=0;
    if(h->read_monotonic_ms){if(h->read_monotonic_ms(h->context,&ms)==0){s.milliseconds=ms;s.available|=SPMP_DIAG_TIME;}
                             else s.failed|=SPMP_DIAG_TIME;}
    int present=0;
    if(h->read_sd_presence){if(h->read_sd_presence(h->context,&present)==0 && (present==0||present==1)){
        s.sd_present=(uint8_t)present;s.available|=SPMP_DIAG_SD;
    }else s.failed|=SPMP_DIAG_SD;}
    uint16_t w=0,t=0;
    if(h->read_lcd_info){if(h->read_lcd_info(h->context,&w,&t)==0 && w>0 && t>0 && w<=4096 && t<=4096){
        s.lcd_width=w;s.lcd_height=t;s.available|=SPMP_DIAG_LCD;
    }else s.failed|=SPMP_DIAG_LCD;}
    uint32_t hz=0;uint8_t channels=0;
    if(h->read_audio_info){if(h->read_audio_info(h->context,&hz,&channels)==0 && hz>=8000 && hz<=192000 && (channels==1||channels==2)){
        s.sample_rate_hz=hz;s.audio_channels=channels;s.available|=SPMP_DIAG_AUDIO;
    }else s.failed|=SPMP_DIAG_AUDIO;}
    *dst=s;return 0;
}
