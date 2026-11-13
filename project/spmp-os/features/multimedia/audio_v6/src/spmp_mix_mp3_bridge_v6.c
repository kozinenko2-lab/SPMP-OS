#include "spmp_mix_decoders_v6.h"
/* Producer never blocks or drops a partial MP3 frame. Accepts rate/channel
 * autodetection on first callback (expected_rate/channels ==0).
 * A stereo ring can hold mono frames duplicated into both output channels.
 */
int spmix_mp3_push(void *context,const int16_t *pcm,size_t frames,unsigned rate,unsigned ch){
 spmix_mp3_bridge *b=(spmix_mp3_bridge *)context;
 spmix_ring *q;size_t i;unsigned pos;
 if(!b||!b->ring||!pcm||!frames||frames>0xffffffffu||
    rate<8000||rate>192000||(ch!=1&&ch!=2))return -1;
 q=b->ring;
 if(!b->expected_rate)b->expected_rate=rate;
 if(!b->expected_channels)b->expected_channels=ch;
 if(rate!=b->expected_rate || ch!=b->expected_channels)return -1;
 if(q->channels!=ch && !(q->channels==2 && ch==1))return -1;
 if(frames>q->capacity_frames-q->count||q->closed)return -1;
 if(q->channels==ch){
  if(spmix_ring_write(q,pcm,(unsigned)frames)!=(unsigned)frames)return -1;
 }else{
  for(i=0;i<frames;i++){
   pos=(q->write_pos+(unsigned)i)%q->capacity_frames;
   q->pcm[pos*2u]=pcm[i];q->pcm[pos*2u+1u]=pcm[i];
  }
  q->write_pos=(q->write_pos+(unsigned)frames)%q->capacity_frames;
  q->count+=(unsigned)frames;
 }
 b->queued_frames+=frames;
 return 0;
}
