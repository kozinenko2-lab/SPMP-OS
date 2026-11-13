#include "spmp_mixer_v6.h"
#include "spmp_mix_decoders_v6.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
typedef struct {const int16_t *data;unsigned frames,channels,ptr,chunk,fail_after;} seq;
static int pull(void*ct,int16_t*dst,unsigned max){
 seq*s=ct;unsigned i,n=s->frames-s->ptr;
 if(s->fail_after && s->ptr>=s->fail_after)return -1;
 if(n>max)n=max;
 if(s->chunk && n>s->chunk)n=s->chunk;
 if(!n)return 0;
 for(i=0;i<n*s->channels;i++)dst[i]=s->data[s->ptr*s->channels+i];
 s->ptr+=n;return (int)n;
}
static void test_basic(void){
 spmix_mixer m;int16_t data[128],out[256];seq s={data,128,1,0,8,0};unsigned i;
 for(i=0;i<128;i++)data[i]=1000;
 CHECK(!spmix_init(&m,44100));CHECK(!spmix_attach(&m,0,pull,&s,44100,1,256,0));
 CHECK(spmix_render(&m,out,128)==128);
 for(i=0;i<128;i++){CHECK(out[2*i]==1000);CHECK(out[2*i+1]==1000);}
 CHECK(spmix_render(&m,out,8)==8);
 for(i=0;i<8;i++)CHECK(out[2*i]==0&&out[2*i+1]==0);
 CHECK(m.refill_calls>1);CHECK(spmix_active_voices(&m)==0);
 puts("PASS constant mono, cache refills, EOF");
}
static void test_resample(void){
 spmix_mixer m;int16_t in[3]={0,1000,2000},out[16];
 seq s={in,3,1,0,2,0};const int16_t exp[6]={0,500,1000,1500,2000,2000};unsigned i;
 CHECK(!spmix_init(&m,16000));CHECK(!spmix_attach(&m,0,pull,&s,8000,1,256,0));
 spmix_render(&m,out,8);
 for(i=0;i<6;i++){if(out[2*i]!=exp[i])fprintf(stderr,"resample at %u expected %d got %d\n",i,exp[i],out[2*i]);CHECK(out[2*i]==exp[i]);CHECK(out[2*i+1]==exp[i]);}
 CHECK(out[12]==0 && out[13]==0 && out[14]==0);
 puts("PASS 2x resampling, end duration");
}
static void test_pan_volume(void){
 spmix_mixer m;int16_t in[16],out[16];seq s={in,8,2,0,0,0};unsigned i;
 for(i=0;i<8;i++){in[2*i]=2000;in[2*i+1]=-1000;}
 CHECK(!spmix_init(&m,44100));CHECK(!spmix_attach(&m,2,pull,&s,44100,2,128,128));
 spmix_render(&m,out,4);
 for(i=0;i<4;i++){CHECK(out[2*i]==0);CHECK(out[2*i+1]==-500);}
 CHECK(!spmix_set_pan(&m,2,-128));spmix_render(&m,out,2);
 for(i=0;i<2;i++){CHECK(out[2*i]==1000&&out[2*i+1]==0);}
 spmix_set_master_gain(&m,128);spmix_render(&m,out,2);
 for(i=0;i<2;i++){CHECK(out[2*i]==500&&out[2*i+1]==0);}
 puts("PASS stereo pan, gains, master volume");
}
static void test_fade(void){
 spmix_mixer m;int16_t in[24],out[16];seq s={in,24,1,0,0,0};int want[]={750,500,250,0};unsigned i;
 for(i=0;i<24;i++)in[i]=1000;
 CHECK(!spmix_init(&m,44100));CHECK(!spmix_attach(&m,0,pull,&s,44100,1,256,0));
 CHECK(!spmix_set_gain(&m,0,0,4));spmix_render(&m,out,4);
 for(i=0;i<4;i++){CHECK(out[2*i]==want[i]);CHECK(out[2*i+1]==want[i]);}
 CHECK(!spmix_set_gain(&m,0,256,0));spmix_render(&m,out,2);
 CHECK(out[0]==1000 && out[2]==1000);
 puts("PASS fade and instantaneous gain");
}
static void test_clipping(void){
 spmix_mixer m;int16_t a[5]={20000,20000,20000,20000,20000},out[12];
 seq s1={a,5,1,0,0,0},s2={a,5,1,0,0,0};unsigned i;
 CHECK(!spmix_init(&m,48000));CHECK(!spmix_attach(&m,0,pull,&s1,48000,1,256,0));
 CHECK(!spmix_attach(&m,1,pull,&s2,48000,1,256,0));spmix_render(&m,out,5);
 for(i=0;i<5;i++)CHECK(out[2*i]==32767&&out[2*i+1]==32767);
 CHECK(m.clipped_samples==10);
 puts("PASS additive mixing and saturation");
}
static void test_ring_mp3(void){
 spmix_ring r;spmix_mixer m;int16_t mem[16],data[16],out[32];
 spmix_mp3_bridge b={0};unsigned i;
 for(i=0;i<16;i++)data[i]=2222;
 CHECK(!spmix_ring_init(&r,mem,8,1));
 CHECK(!spmix_init(&m,48000));CHECK(!spmix_attach(&m,0,spmix_ring_read,&r,48000,1,256,0));
 spmix_render(&m,out,2);
 CHECK(out[0]==0&&out[1]==0&&m.starved_reads>0&&spmix_active_voices(&m)==1);
 b.ring=&r;b.expected_rate=48000;b.expected_channels=1;
 CHECK(!spmix_mp3_push(&b,data,7,48000,1));
 CHECK(spmix_mp3_push(&b,data,3,48000,1)!=0);
 CHECK(r.count==7);
 spmix_render(&m,out,6);
 for(i=0;i<6;i++)CHECK(out[2*i]==2222);
 CHECK(!spmix_mp3_push(&b,data,4,48000,1));
 spmix_ring_close(&r);
 spmix_render(&m,out,6);
 for(i=0;i<5;i++)CHECK(out[2*i]==2222);
 CHECK(out[10]==0);
 CHECK(b.queued_frames==11);
 puts("PASS MP3 PCM push, starvation, FIFO, closing");
}
static void test_errors_and_bounds(void){
 spmix_mixer m;int16_t in[8]={1,2,3,4,5,6,7,8};int16_t out[1024];seq s={in,8,1,0,0,2};
 CHECK(spmix_init(&m,1)!=0);CHECK(!spmix_init(&m,192000));
 CHECK(spmix_attach(&m,0,pull,&s,0,1,256,0)!=0);
 CHECK(spmix_attach(&m,4,pull,&s,8000,1,256,0)!=0);
 CHECK(spmix_attach(&m,0,pull,&s,8000,3,256,0)!=0);
 CHECK(!spmix_attach(&m,0,pull,&s,8000,1,256,0));
 CHECK(spmix_set_gain(&m,0,257,100)!=0);
 CHECK(spmix_set_gain(&m,0,128,0x80000000u)!=0);
 spmix_render(&m,out,4);
 CHECK(m.input_errors==0); /* 8000->192000: still in first 2 samples */
 spmix_render(&m,out,400);
 CHECK(m.input_errors>=1);
 CHECK(spmix_active_voices(&m)==0);
 puts("PASS input validation and negative reader errors");
}
static int sink_open(void*ctx,uint32_t r,unsigned c){unsigned*counter=ctx;CHECK(r==44100 && c==2);(*counter)++;return 0;}
static int sink_write(void*ctx,const int16_t*p,size_t n){unsigned*counter=ctx;CHECK(p&&n==4);(*counter)++;return 0;}
static void sink_close(void*ctx){unsigned*counter=ctx;(*counter)++;}
static void test_sink(void){
 spmix_mixer m;spmix_sink s;int16_t out[8];unsigned count=0;
 CHECK(!spmix_init(&m,44100));s.ctx=&count;s.open=sink_open;s.write=sink_write;s.close=sink_close;
 CHECK(!spmix_sink_open(&s,&m));CHECK(!spmix_pump(&s,&m,out,4));spmix_sink_close(&s);
 CHECK(count==3);CHECK(out[0]==0);puts("PASS output HAL sink interface");
}
static uint32_t rng=0x87654321u;
static uint32_t next_rand(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static void test_fuzz(void){
 int16_t input[2*130];unsigned trial;
 for(trial=0;trial<1500;trial++){
  spmix_mixer m;seq s;unsigned i,ch=(next_rand()&1)?1:2,rate=(next_rand()%3==0)?8000:((next_rand()%2)?44100:48000);
  unsigned len=next_rand()%128+1;int16_t out[2*140+4];
  for(i=0;i<130*2;i++)input[i]=(int16_t)next_rand();
  s.data=input;s.frames=len;s.channels=ch;s.ptr=0;s.chunk=next_rand()%19+1;s.fail_after=0;
  CHECK(!spmix_init(&m,44100));
  CHECK(!spmix_attach(&m,0,pull,&s,rate,ch,next_rand()%257,(int)(next_rand()%257)-128));
  out[0]=1234;out[1]=-2345;out[282]=1111;out[283]=-1111;
  CHECK(spmix_render(&m,out+2,140)==140);
  CHECK(out[0]==1234&&out[1]==-2345&&out[282]==1111&&out[283]==-1111);
 }
 puts("PASS randomized frame ranges, boundary canaries (1500 runs)");
}
int main(void){test_basic();test_resample();test_pan_volume();test_fade();test_clipping();test_ring_mp3();test_errors_and_bounds();test_sink();test_fuzz();puts("ALL MIXER TESTS PASS");return 0;}
