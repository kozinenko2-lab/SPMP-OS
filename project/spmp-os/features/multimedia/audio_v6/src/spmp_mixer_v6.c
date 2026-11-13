#include "spmp_mixer_v6.h"
#include <string.h>

static int ok_rate(uint32_t r){return r>=8000u && r<=192000u;}
static int ok_voice(spmix_mixer *m,unsigned n){return m && ok_rate(m->output_rate) && n<SPMIX_MAX_VOICES;}
static int16_t limit16(spmix_mixer *m,int64_t value){
 if(value>32767){m->clipped_samples++;return 32767;}
 if(value<(-32768)){m->clipped_samples++;return -32768;}
 return (int16_t)value;
}
int spmix_init(spmix_mixer *m,uint32_t rate){
 if(!m || !ok_rate(rate))return -1;
 memset(m,0,sizeof(*m));m->output_rate=rate;m->master_gain_q8=256;return 0;
}
int spmix_attach(spmix_mixer *m,unsigned slot,spmix_pull_fn cb,void *ctx,
                 uint32_t rate,unsigned ch,unsigned gain,int pan){
 spmix_voice *v;
 if(!ok_voice(m,slot) || !cb || !ok_rate(rate) || (ch!=1&&ch!=2) || gain>256 || pan < -128 || pan >128)return -1;
 v=&m->voice[slot];memset(v,0,sizeof(*v));v->pull=cb;v->user=ctx;
 v->input_rate=rate;v->channels=(uint8_t)ch;v->step_q32=((uint64_t)rate<<32)/m->output_rate;
 if(v->step_q32==0) v->step_q32=1;
 v->gain_q16=(int32_t)(gain<<16);v->target_gain_q8=(uint16_t)gain;v->pan=(int16_t)pan;v->active=1;
 return 0;
}
void spmix_detach(spmix_mixer *m,unsigned slot){
 if(ok_voice(m,slot))memset(&m->voice[slot],0,sizeof(m->voice[slot]));
}
int spmix_set_pan(spmix_mixer *m,unsigned slot,int pan){
 if(!ok_voice(m,slot)||pan < -128||pan>128)return -1;
 m->voice[slot].pan=(int16_t)pan;return 0;
}
void spmix_set_master_gain(spmix_mixer *m,unsigned gain){if(m)m->master_gain_q8=(uint16_t)(gain>256?256:gain);}
int spmix_set_gain(spmix_mixer *m,unsigned slot,unsigned target,uint32_t ramp_frames){
 spmix_voice *v;
 if(!ok_voice(m,slot)||target>256||ramp_frames>0x7fffffffu)return -1;
 v=&m->voice[slot];v->target_gain_q8=(uint16_t)target;
 if(ramp_frames==0){v->gain_q16=(int32_t)(target<<16);v->fade_left=0;v->fade_step_q16=0;return 0;}
 v->fade_left=ramp_frames;
 v->fade_step_q16=((int32_t)(target<<16)-v->gain_q16)/(int32_t)ramp_frames;
 return 0;
}
static void fade_one(spmix_voice *v){
 if(!v->fade_left)return;
 --v->fade_left;
 if(v->fade_left==0)v->gain_q16=(int32_t)v->target_gain_q8<<16;
 else v->gain_q16+=v->fade_step_q16;
}
unsigned spmix_active_voices(const spmix_mixer *m){
 unsigned i,n=0;if(!m)return 0;
 for(i=0;i<SPMIX_MAX_VOICES;i++)n+=(m->voice[i].active!=0);
 return n;
}
/* Fetch exactly one input frame, using 256-frame bursts to amortize decoder calls. */
static int pop(spmix_mixer *m,spmix_voice *v,int16_t x[2]){
 if(v->cache_index>=v->cache_length){
  int n=v->pull(v->user,v->cache,SPMIX_FETCH_FRAMES);
  m->refill_calls++;
  if(n==SPMIX_STARVED){m->starved_reads++;return SPMIX_STARVED;}
  if(n<0 || n>(int)SPMIX_FETCH_FRAMES){m->input_errors++;v->active=0;return -1;}
  if(n==0)return 0;
  v->cache_index=0;v->cache_length=(unsigned)n;
 }
 x[0]=v->cache[v->cache_index*v->channels];
 x[1]=v->channels==2?v->cache[v->cache_index*2u+1u]:x[0];
 v->cache_index++;return 1;
}
static int prime(spmix_mixer *m,spmix_voice *v){
 int r;
 if(!v->have_a){
  r=pop(m,v,v->a);
  if(r==0){v->active=0;return 0;}
  if(r<0)return r;
  v->have_a=1;
 }
 if(!v->have_b){
  r=pop(m,v,v->b);
  if(r==0){v->b[0]=v->a[0];v->b[1]=v->a[1];v->terminal=1;}
  else if(r<0)return r;
  v->have_b=1;
 }
 return 1;
}
static int position(spmix_mixer *m,spmix_voice *v){
 int r;
 while(v->phase_q32>=((uint64_t)1<<32)){
  if(v->terminal){v->active=0;return 0;}
  if(!v->have_b){r=prime(m,v);if(r<=0)return r;}
  v->phase_q32-=((uint64_t)1<<32);
  v->a[0]=v->b[0];v->a[1]=v->b[1];v->have_a=1;v->have_b=0;
  r=prime(m,v);if(r<=0)return r;
 }
 return 1;
}
size_t spmix_render(spmix_mixer *m,int16_t *pcm,size_t frames){
 size_t frame;unsigned i;
 if(!m||!pcm||!ok_rate(m->output_rate))return 0;
 for(frame=0;frame<frames;frame++){
  int64_t left=0,right=0;
  for(i=0;i<SPMIX_MAX_VOICES;i++){
   spmix_voice *v=&m->voice[i];int32_t s0,s1,l,r,g,lg,rg;
   if(!v->active)continue;
   fade_one(v);
   if(prime(m,v)<=0||position(m,v)<=0)continue;
   g=v->gain_q16>>16;
   if(g<=0){v->phase_q32+=v->step_q32;continue;}
   s0=(int32_t)v->a[0]+((int32_t)(((int64_t)((int32_t)v->b[0]-(int32_t)v->a[0])*(int64_t)(v->phase_q32>>16))>>16));
   s1=(int32_t)v->a[1]+((int32_t)(((int64_t)((int32_t)v->b[1]-(int32_t)v->a[1])*(int64_t)(v->phase_q32>>16))>>16));
   lg=g;rg=g;
   if(v->pan>0)lg=(g*(128-v->pan))>>7;
   else if(v->pan<0)rg=(g*(128+v->pan))>>7;
   l=(s0*lg)>>8;r=(s1*rg)>>8;
   left+=l;right+=r;
   v->phase_q32+=v->step_q32;
  }
  left=left*m->master_gain_q8/256;
  right=right*m->master_gain_q8/256;
  pcm[frame*2u]=limit16(m,left);pcm[frame*2u+1u]=limit16(m,right);
 }
 m->frames_rendered+=frames;return frames;
}
int spmix_ring_init(spmix_ring *q,int16_t*pcm,unsigned frames,unsigned channels){
 if(!q||!pcm||frames<2||frames>0x7fffffffu||(channels!=1&&channels!=2))return -1;
 memset(q,0,sizeof(*q));q->pcm=pcm;q->capacity_frames=frames;q->channels=channels;return 0;
}
unsigned spmix_ring_write(spmix_ring *q,const int16_t *in,unsigned frames){
 unsigned i,n;if(!q||!q->pcm||!in||q->closed)return 0;
 n=q->capacity_frames-q->count;
 if(n>frames)n=frames;
 for(i=0;i<n;i++){
  unsigned p=(q->write_pos+i)%q->capacity_frames;
  q->pcm[p*q->channels]=in[i*q->channels];
  if(q->channels==2)q->pcm[p*2u+1u]=in[i*2u+1u];
 }
 q->count+=n;q->write_pos=(q->write_pos+n)%q->capacity_frames;
 if(n<frames)q->overruns++;
 return n;
}
int spmix_ring_read(void *context,int16_t *dst,unsigned max_frames){
 spmix_ring *q=(spmix_ring*)context;unsigned i,n;
 if(!q||!q->pcm||!dst||max_frames==0)return -1;
 n=q->count;if(n>max_frames)n=max_frames;
 for(i=0;i<n;i++){
  unsigned p=(q->read_pos+i)%q->capacity_frames;
  dst[i*q->channels]=q->pcm[p*q->channels];
  if(q->channels==2)dst[i*2u+1u]=q->pcm[p*2u+1u];
 }
 q->count-=n;q->read_pos=(q->read_pos+n)%q->capacity_frames;
 if(n==0)return q->closed?0:SPMIX_STARVED;
 return (int)n;
}
void spmix_ring_close(spmix_ring *q){if(q)q->closed=1;}
void spmix_ring_reset(spmix_ring *q){
 if(q){q->read_pos=q->write_pos=q->count=q->closed=0;q->overruns=0;}
}
int spmix_sink_open(spmix_sink *s,spmix_mixer *m){
 if(!m||!s||!s->open||!s->write)return -1;
 return s->open(s->ctx,m->output_rate,2u);
}
int spmix_pump(spmix_sink *s,spmix_mixer *m,int16_t *buf,size_t frames){
 if(!s||!s->write||!m||!buf||!frames)return -1;
 if(spmix_render(m,buf,frames)!=frames)return -1;
 return s->write(s->ctx,buf,frames);
}
void spmix_sink_close(spmix_sink *s){if(s&&s->close)s->close(s->ctx);}
