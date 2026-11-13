#include "spmp_small_video.h"
#include <string.h>
int spmp_video_fit(unsigned iw,unsigned ih,unsigned mw,unsigned mh,unsigned *ow,unsigned *oh){
 if(!iw||!ih||!mw||!mh||!ow||!oh||iw>4096||ih>4096)return -1;
 if(iw<=mw&&ih<=mh){*ow=iw;*oh=ih;return 0;}
 if((uint64_t)iw*mh >= (uint64_t)ih*mw){
  *ow=mw;*oh=(unsigned)(((uint64_t)ih*mw)/iw);
 }else{
  *oh=mh;*ow=(unsigned)(((uint64_t)iw*mh)/ih);
 }
 if(!*ow)*ow=1;
 if(!*oh)*oh=1;
 return 0;
}
int spmp_video_convert(const void *source,size_t pitch,unsigned iw,unsigned ih,
 unsigned format,uint16_t *dest,unsigned ow,unsigned oh){
 if(!source||!dest||!iw||!ih||!ow||!oh||!pitch||iw>4096||ih>4096||ow>iw||oh>ih||format>2)return -1;
 const unsigned bpp=format==SPMP_VIDEO_XRGB8888?4u:2u;
 if(pitch<(size_t)iw*bpp)return -1;
 const uint8_t *base=(const uint8_t*)source;
 for(unsigned y=0;y<oh;y++){
  const unsigned sy=(unsigned)(((uint64_t)y*ih)/oh);
  const uint8_t *row=base+(size_t)sy*pitch;
  uint16_t *out=dest+(size_t)y*ow;
  if(ow==iw&&format==SPMP_VIDEO_RGB565){memcpy(out,row,(size_t)iw*2u);continue;}
  for(unsigned x=0;x<ow;x++){
   unsigned sx=(unsigned)(((uint64_t)x*iw)/ow);
   if(format==SPMP_VIDEO_XRGB8888){
    uint32_t v;memcpy(&v,row+(size_t)sx*4u,4);
    out[x]=(uint16_t)((((v>>19)&31u)<<11)|(((v>>10)&63u)<<5)|((v>>3)&31u));
   }else{
    uint16_t v;memcpy(&v,row+(size_t)sx*2u,2);
    if(format==SPMP_VIDEO_RGB565)out[x]=v;
    else out[x]=(uint16_t)(((v&0x7c00u)<<1)|((v&0x03e0u)<<1)|(v&0x001fu));
   }
  }
 }
 return 0;
}
