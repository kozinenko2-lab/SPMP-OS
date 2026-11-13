#include "spmp_audio_stage10.h"
#include <stdio.h>
static int opened,written,closed;
static int o(void*c,unsigned r,unsigned ch){(void)c;if(r!=44100||ch!=2)return -1;opened++;return 0;}
static int w(void*c,const int16_t*p,size_t n){(void)c;if(!p||n!=10)return -1;written++;return 0;}
static int cl(void*c){(void)c;closed++;return 0;}
int main(void){spmp_multimedia_driver hal={0};sp_audio_stage10_ctx ctx;sp_audio_sink sink;int16_t x[20]={0};
 hal.audio_open=o;hal.audio_write_s16=w;hal.audio_close=cl;
 sp_audio_stage10_bind(&sink,&ctx,&hal);
 if(sink.open(sink.ctx,44100,2)||sink.write(sink.ctx,x,10))return 1;
 sink.close(sink.ctx);
 if(opened!=1||written!=1||closed!=1)return 2;
 puts("Stage10 adapter OK");return 0;}
