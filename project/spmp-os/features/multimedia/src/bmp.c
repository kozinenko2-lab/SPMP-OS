#include "common.h"
/* BMP preview is intentionally RAM-bounded; row image decoding is streaming. */
int mm_render_bmp(const mm_reader *r,uint16_t *fb,unsigned pitch,unsigned sw,unsigned sh){
 uint8_t h[54];uint8_t row[4096];
 uint32_t width,height,offset,dib,bpp,row_bytes,raw_h;
 int top_down;
 if(!r||!fb||!sw||!sh||sw>1024||sh>1024||pitch<sw||
    r->size<54||mm_read(r,0,h,54))return -1;
 if(h[0]!='B'||h[1]!='M')return -1;
 dib=rd32(h+14);offset=rd32(h+10);width=rd32(h+18);raw_h=rd32(h+22);
 top_down=(raw_h&0x80000000u)!=0;height=top_down?(~raw_h+1u):raw_h;
 bpp=rd16(h+28);
 if(dib<40||rd16(h+26)!=1||rd32(h+30)!=0||
    (bpp!=24&&bpp!=32)||width==0||height==0||width>1024||
    height>SPMP_MM_MAX_PHOTO_SIDE||
    width*height>SPMP_MM_MAX_PHOTO_PIXELS)return -1;
 row_bytes=((width*(bpp/8u))+3u)&~3u;
 if(row_bytes>sizeof row||offset>r->size||
    (uint64_t)row_bytes*height>r->size-offset||
    offset<14u+dib)return -1;
 /* Fit, preserving aspect ratio, without floating point or large buffers. */
 unsigned dw=sw,dh=(unsigned)(height*sw/width);
 if(dh>sh){dh=sh;dw=(unsigned)(width*sh/height);}
 if(!dw)dw=1;
 if(!dh)dh=1;
 unsigned x0=(sw-dw)/2u,y0=(sh-dh)/2u;
 for(unsigned y=0;y<sh;y++)for(unsigned x=0;x<sw;x++)fb[(size_t)y*pitch+x]=0;
 uint32_t prior_y=UINT32_MAX;
 for(unsigned y=0;y<dh;y++){
  uint32_t iy=(uint32_t)(y*height/dh);
  uint32_t src_y=top_down?iy:height-1u-iy;
  if(src_y!=prior_y){
   if(mm_read(r,offset+(uint64_t)src_y*row_bytes,row,row_bytes))return -1;
   prior_y=src_y;
  }
  for(unsigned x=0;x<dw;x++){
   uint32_t ix=(uint32_t)(x*width/dw);
   const uint8_t *p=row+ix*(bpp/8u);
   fb[(size_t)(y0+y)*pitch+x0+x]=mm_rgb565(p[2],p[1],p[0]);
  }
 }
 return 0;
}
