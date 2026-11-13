#include "spmp_mix_stage10_v6.h"
static int begin(void *user,uint32_t rate,unsigned ch){
 spmix_stage10_context *s=(spmix_stage10_context*)user;
 if(!s||!s->hal||!s->hal->audio_open||!s->hal->audio_write_s16||ch!=2)return -1;
 if(s->active)return -1;
 if(s->hal->audio_open(s->hal->context,rate,ch)!=0)return -1;
 s->active=1;return 0;
}
static int output(void *user,const int16_t *pcm,size_t frames){
 spmix_stage10_context *s=(spmix_stage10_context*)user;
 return !s||!s->active||!s->hal||!s->hal->audio_write_s16?-1:
  s->hal->audio_write_s16(s->hal->context,pcm,frames);
}
static void finish(void *user){
 spmix_stage10_context *s=(spmix_stage10_context*)user;
 if(s && s->active && s->hal && s->hal->audio_close)
  s->hal->audio_close(s->hal->context);
 if(s)s->active=0;
}
void spmix_bind_stage10(spmix_sink *sink,spmix_stage10_context *ctx,const spmp_multimedia_driver *hal){
 if(!sink||!ctx)return;
 ctx->hal=hal;ctx->active=0;
 sink->ctx=ctx;sink->open=begin;sink->write=output;sink->close=finish;
}
