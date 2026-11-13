#include "spmp_mm_stage10.h"
static int submit(void *ctx,const int16_t *pcm,size_t frames,unsigned rate){
 mm_stage10_audio *a=(mm_stage10_audio*)ctx;
 if(!a||!a->platform||!a->platform->audio_submit_s16)return -1;
 return a->platform->audio_submit_s16(a->platform->hardware,pcm,frames,rate);
}
void mm_stage10_bind_audio(mm_audio_output *dst,mm_stage10_audio *adapter,
                           const defos_hal *platform){
 if(!dst||!adapter)return;
 adapter->platform=platform;
 dst->ctx=adapter;dst->submit_s16=submit;
}
