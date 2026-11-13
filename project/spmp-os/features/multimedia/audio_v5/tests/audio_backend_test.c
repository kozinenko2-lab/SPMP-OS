#include "spmp_audio_v5.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static int nopen,ndecode,nseek,nclose;
static int rd(void*x,void*d,unsigned n){(void)x;(void)d;(void)n;return 0;}
static int op(void*c,sp_audio_io io,void*w,size_t wb,sp_audio_info*i){(void)c;(void)io;(void)w;(void)wb;
 nopen++;i->rate=8000;i->channels=1;i->bits=16;return 0;}
static int dec(void*c,int16_t*o,size_t cap,size_t*n){(void)c;ndecode++;if(cap<4)return SP_AUD_NO_MEMORY;
 for(int j=0;j<4;j++)o[j]=1234;*n=4;return 0;}
static int seek_(void*c,uint64_t ms){(void)c;(void)ms;nseek++;return 0;}
static void close_(void*c){(void)c;nclose++;}
int main(void){sp_audio_io io={0};sp_audio_external_backend b={0};sp_audio_player*p=calloc(1,sp_audio_player_size());
 int16_t samples[8]={0};int dummy=42;size_t n=0;
 io.read=rd;b.open=op;b.decode=dec;b.seek_ms=seek_;b.close=close_;
 if(sp_audio_open(p,io,SP_FMT_WMA_ASF,&dummy,sizeof(dummy))!=SP_AUD_NEED_BACKEND)return 1;
 if(sp_audio_open_with_backend(p,io,SP_FMT_WMA_ASF,&dummy,sizeof(dummy),&b))return 2;
 if(sp_audio_decode(p,samples,8,&n)||n!=4||samples[0]!=1234)return 3;
 if(sp_audio_seek_ms(p,120))return 4;
 sp_audio_close(p);free(p);
 if(nopen!=1||ndecode!=1||nseek!=1||nclose!=1)return 5;
 printf("external backend dispatch OK\n");return 0;
}
