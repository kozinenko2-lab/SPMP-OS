#include "spmp_mix_stage10_v6.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static int opens,writes,closes;static size_t frames_seen;
static int audio_open(void*c,unsigned rate,unsigned channels){(void)c;CHECK(rate==44100&&channels==2);opens++;return 0;}
static int audio_write(void*c,const int16_t*pcm,size_t frames){(void)c;CHECK(pcm&&frames==256);writes++;frames_seen+=frames;return 0;}
static int audio_close(void*c){(void)c;closes++;return 0;}
int main(void){spmp_multimedia_driver h={0};spmix_stage10_context c={0};spmix_sink s;spmix_mixer m;int16_t buf[512];
 h.audio_open=audio_open;h.audio_write_s16=audio_write;h.audio_close=audio_close;
 CHECK(!spmix_init(&m,44100));spmix_bind_stage10(&s,&c,&h);
 CHECK(!spmix_sink_open(&s,&m));CHECK(spmix_sink_open(&s,&m)!=0);
 CHECK(!spmix_pump(&s,&m,buf,256));CHECK(!spmix_pump(&s,&m,buf,256));
 spmix_sink_close(&s);CHECK(opens==1&&writes==2&&closes==1&&frames_seen==512);
 puts("PASS real Stage10 HAL interface, open/pump/close");return 0;
}
