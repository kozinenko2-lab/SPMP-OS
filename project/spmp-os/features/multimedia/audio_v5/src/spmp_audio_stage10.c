#include "spmp_audio_stage10.h"
static int start(void *ctx,uint32_t rate,unsigned channels){sp_audio_stage10_ctx*s=ctx;
 if(!s||!s->hal||!s->hal->audio_open||!s->hal->audio_write_s16)return SP_AUD_BAD;
 if(s->active && (s->rate!=rate||s->channels!=channels)){if(s->hal->audio_close)s->hal->audio_close(s->hal->context);s->active=0;}
 if(!s->active){int rc=s->hal->audio_open(s->hal->context,rate,channels);
  if(rc)return SP_AUD_IO;s->active=1;s->rate=rate;s->channels=channels;}
 return 0;
}
static int write_pcm(void *ctx,const int16_t*pcm,size_t frames){sp_audio_stage10_ctx*s=ctx;
 return (!s||!s->active||!s->hal||!s->hal->audio_write_s16)?SP_AUD_BAD:
 s->hal->audio_write_s16(s->hal->context,pcm,frames);
}
static void stop(void *ctx){sp_audio_stage10_ctx*s=ctx;
 if(s&&s->active&&s->hal&&s->hal->audio_close)s->hal->audio_close(s->hal->context);
 if(s)s->active=0;
}
void sp_audio_stage10_bind(sp_audio_sink*out,sp_audio_stage10_ctx*ctx,const spmp_multimedia_driver*hal){
 if(!out||!ctx)return;ctx->hal=hal;ctx->rate=ctx->channels=0;ctx->active=0;
 out->ctx=ctx;out->open=start;out->write=write_pcm;out->close=stop;
}
