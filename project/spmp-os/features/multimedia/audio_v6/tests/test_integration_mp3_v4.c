#include "spmp_mixer_v6.h"
#include "spmp_mix_decoders_v6.h"
#include "spmp_mp3_player.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
static int rd(void*u,uint8_t*p,size_t n){FILE*f=(FILE*)u;int r=(int)fread(p,1,n,f);return r?r:ferror(f)?-1:0;}
static int seek_(void*u,uint64_t pos){return pos>2147483647u?-1:fseek(u,(long)pos,SEEK_SET);}
static uint64_t size_(void*u){FILE*f=u;long old=ftell(f),len;if(old<0||fseek(f,0,SEEK_END))return 0;len=ftell(f);fseek(f,old,SEEK_SET);return len<0?0:(uint64_t)len;}
int main(int argc,char**argv){
 spmp_mp3_player *mp3;spmp_mp3_io io;spmix_mixer *mix;spmix_ring queue;spmix_mp3_bridge bridge={0};
 int16_t *ring_buffer;int16_t buf[256*2];FILE *in,*out;unsigned loops=0,done=0;int status;
 if(argc!=3){fprintf(stderr,"usage: %s input.mp3 output.s16\n",argv[0]);return 1;}
 in=fopen(argv[1],"rb");out=fopen(argv[2],"wb");if(!in||!out)return 2;
 mp3=calloc(1,spmp_mp3_player_size());mix=calloc(1,sizeof(*mix));ring_buffer=calloc(8192*2,sizeof(int16_t));
 if(!mp3||!mix||!ring_buffer)return 3;
 if(spmix_ring_init(&queue,ring_buffer,8192,2))return 4;
 bridge.ring=&queue;
 io.ctx=in;io.read=rd;io.seek=seek_;io.size=size_;
 if(spmp_mp3_open(mp3,&io,spmix_mp3_push,&bridge))return 5;
 /* Decode until sample format is known — no changes to v4 decoder. */
 while(!bridge.expected_rate && loops++<100){status=spmp_mp3_step(mp3);if(status<0)return 6;if(!status)break;}
 if(!bridge.expected_rate)return 7;
 if(spmix_init(mix,bridge.expected_rate)||spmix_attach(mix,0,spmix_ring_read,&queue,bridge.expected_rate,2,256,0))return 8;
 loops=0;
 while(loops++<20000){
  while(!done && queue.capacity_frames-queue.count>=1152 && queue.count<4096){
   status=spmp_mp3_step(mp3);
   if(status<0){fprintf(stderr,"MP3 step error=%d\n",status);return 9;}
   if(status==0){done=1;spmix_ring_close(&queue);break;}
  }
  if(!spmix_active_voices(mix))break;
  spmix_render(mix,buf,256);
  if(fwrite(buf,sizeof(int16_t),512,out)!=512)return 10;
 }
 fprintf(stderr,"MP3->Mixer rate=%u source_ch=%u pushed_frames=%llu output_frames=%llu starved=%u input_errors=%u\n",
 bridge.expected_rate,bridge.expected_channels,(unsigned long long)bridge.queued_frames,
 (unsigned long long)mix->frames_rendered,mix->starved_reads,mix->input_errors);
 if(loops>=20000 ||mix->input_errors||mix->starved_reads||!done)return 11;
 spmp_mp3_close(mp3);fclose(in);fclose(out);free(mp3);free(mix);free(ring_buffer);return 0;
}
