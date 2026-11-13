#include "spmp_mix_decoders_v6.h"
#include "spmp_audio_v5.h"
int spmix_audio_v5_pull(void *ctx,int16_t *dst,unsigned max_frames){
 size_t got=0;
 int rc;
 if(!ctx||!dst||max_frames==0)return -1;
 rc=sp_audio_decode((sp_audio_player*)ctx,dst,(size_t)max_frames,&got);
 if(got>max_frames)return -1;
 if(got)return (int)got;
 if(rc==SP_AUD_EOF)return 0;
 if(rc==SP_AUD_OK)return SPMIX_STARVED;
 return -1;
}

#include "spmp_mix_audio_v5_bridge.h"
#include <string.h>
int spmix_audio_v5_buffered_init(spmix_audio_v5_buffered *ctx,sp_audio_player *p,
                                 int16_t *pcm,unsigned cap,unsigned ch){
 if(!ctx||!p||!pcm||cap<256||(ch!=1&&ch!=2))return -1;
 memset(ctx,0,sizeof(*ctx));ctx->player=p;ctx->pcm=pcm;
 ctx->capacity_frames=cap;ctx->channels=ch;return 0;
}
int spmix_audio_v5_buffered_pull(void *context,int16_t *dst,unsigned cap){
 spmix_audio_v5_buffered *s=(spmix_audio_v5_buffered*)context;
 unsigned total=0;
 if(!s||!s->player||!s->pcm||!dst||cap==0)return -1;
 while(total<cap){
  if(s->offset_frames<s->buffered_frames){
   unsigned n=s->buffered_frames-s->offset_frames;
   if(n>cap-total)n=cap-total;
   memcpy(dst+total*s->channels,s->pcm+s->offset_frames*s->channels,
          n*s->channels*sizeof(int16_t));
   s->offset_frames+=n;total+=n;
   continue;
  }
  {
   size_t n=0;int rc=sp_audio_decode(s->player,s->pcm,s->capacity_frames,&n);
   if(n>s->capacity_frames){s->decoder_errors++;return -1;}
   if(n==0){
    if(rc==SP_AUD_EOF)return total?(int)total:0;
    if(rc==SP_AUD_OK)return total?(int)total:SPMIX_STARVED;
    s->decoder_errors++;return total?(int)total:-1;
   }
   s->offset_frames=0;s->buffered_frames=(unsigned)n;
  }
 }
 return (int)total;
}
