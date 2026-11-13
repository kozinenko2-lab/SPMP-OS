#include "spmp_mp3_stage10.h"
void spmp_mp3_stage10_init(spmp_mp3_stage10_sink *s,const spmp_multimedia_driver *hal){
 if(!s)return;s->hal=hal;s->opened=0;s->rate=s->channels=0;
}
int spmp_mp3_stage10_pcm(void *ctx,const int16_t *pcm,size_t frames,unsigned rate,unsigned channels){
 spmp_mp3_stage10_sink *s=(spmp_mp3_stage10_sink *)ctx;
 if(!s || !s->hal || !s->hal->audio_open || !s->hal->audio_write_s16)return -1;
 if(s->opened && (s->rate!=rate || s->channels!=channels)){
  if(s->hal->audio_close)s->hal->audio_close(s->hal->context);s->opened=0;
 }
 if(!s->opened){
  if(s->hal->audio_open(s->hal->context,rate,channels)!=0)return -1;
  s->rate=rate;s->channels=channels;s->opened=1;
 }
 return s->hal->audio_write_s16(s->hal->context,pcm,frames);
}
void spmp_mp3_stage10_close(spmp_mp3_stage10_sink *s){
 if(!s)return;if(s->opened && s->hal && s->hal->audio_close)s->hal->audio_close(s->hal->context);
 s->opened=0;
}
