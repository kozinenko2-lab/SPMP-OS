#include "spmp_small_video.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){
 unsigned w=0,h=0;
 assert(!spmp_video_fit(512,448,320,240,&w,&h));
 assert(w==274&&h==240); /* retain aspect ratio and cap LCD area */
 assert(!spmp_video_fit(256,224,320,240,&w,&h)&&w==256&&h==224);
 assert(!spmp_video_fit(512,224,320,240,&w,&h)&&w==320&&h==140);
 assert(spmp_video_fit(0,240,320,240,&w,&h)==-1);
 uint16_t *src=calloc((size_t)512*448,sizeof(uint16_t));
 uint16_t *dst=calloc((size_t)320*240,sizeof(uint16_t));
 assert(src&&dst);
 for(unsigned y=0;y<448;y++)for(unsigned x=0;x<512;x++)src[y*512+x]=(uint16_t)(x+y*3u);
 assert(!spmp_video_fit(512,448,320,240,&w,&h));
 assert(!spmp_video_convert(src,512*2u,512,448,SPMP_VIDEO_RGB565,dst,w,h));
 assert(dst[0]==src[0]);
 assert(dst[(h-1)*w+w-1]==src[((h-1)*448u/h)*512u+((w-1)*512u/w)]);
 assert(spmp_video_convert(src,1,512,448,SPMP_VIDEO_RGB565,dst,w,h)==-1);
 uint16_t blue=0x7c00,one=0;
 assert(!spmp_video_convert(&blue,2,1,1,SPMP_VIDEO_0RGB1555,&one,1,1));
 assert(one==0xf800); /* 1555 red -> RGB565 red */
 uint32_t px=0x00ff8000;
 assert(!spmp_video_convert(&px,4,1,1,SPMP_VIDEO_XRGB8888,&one,1,1));
 assert((one&0xf800)==0xf800);
 free(src);free(dst);
 puts("Stage23: SNES HiRes LCD scaler / RGB565 / 1555 / 8888 passed");
}
