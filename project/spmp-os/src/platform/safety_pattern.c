#include "defos_hal.h"
/* Freestanding ARMv5 component: no MMIO and no libc. Caller supplies RAM. */
void defos_draw_safety_pattern(uint16_t *dst,unsigned width,unsigned height,
                               unsigned pitch_pixels,unsigned frame_number) {
 if(!dst||width==0||height==0||pitch_pixels<width||width>4096||height>4096)return;
 for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x){
   unsigned k=(x*3/width + frame_number/120)%3;
   uint16_t c=k==0?0xf800:(k==1?0x07e0:0x001f);
   if(y<12||y>=height-12||x<8||x>=width-8)c=0xffff;
   dst[y*pitch_pixels+x]=c;
 }
}
