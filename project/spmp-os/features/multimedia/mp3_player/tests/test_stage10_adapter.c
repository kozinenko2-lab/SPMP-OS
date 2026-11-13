#include "spmp_mp3_stage10.h"
#include <assert.h>
#include <stdio.h>
static int opens,writes,closes;
static int o(void *c,unsigned hz,unsigned n){(void)c; assert(hz==44100||hz==22050);assert(n==1||n==2);opens++;return 0;}
static int w(void *c,const int16_t *d,size_t n){(void)c; assert(d&&n);writes++;return 0;}
static int cl(void *c){(void)c;closes++;return 0;}
int main(void){spmp_multimedia_driver h={0};spmp_mp3_stage10_sink s;int16_t b[4]={0};
 h.audio_open=o;h.audio_write_s16=w;h.audio_close=cl;spmp_mp3_stage10_init(&s,&h);
 assert(!spmp_mp3_stage10_pcm(&s,b,2,44100,2));
 assert(!spmp_mp3_stage10_pcm(&s,b,2,44100,2));
 assert(!spmp_mp3_stage10_pcm(&s,b,2,22050,1));
 spmp_mp3_stage10_close(&s);assert(opens==2 && writes==3 && closes==2);
 puts("Stage10 bridge PASS");return 0;}
