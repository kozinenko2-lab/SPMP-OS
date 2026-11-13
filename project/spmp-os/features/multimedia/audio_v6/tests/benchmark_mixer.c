#include "spmp_mixer_v6.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
static int ringdata(void*ctx,int16_t*out,unsigned n){
 uint32_t *phase=(uint32_t*)ctx;unsigned i;for(i=0;i<n;i++){
  /* approximate repeating triangle wave, deliberately mono, no FPU */
  uint32_t p=(*phase)++ & 0xffu;
  out[i]=(int16_t)((p<128?p:255-p)*140-8900);
 }
 return (int)n;
}
int main(void){spmix_mixer *m=calloc(1,sizeof(*m));int16_t out[2*1024];unsigned long k;
 uint32_t phase[2]={0,10};clock_t start,end;double sec,frames_per_s;
 if(!m ||spmix_init(m,48000)||spmix_attach(m,0,ringdata,&phase[0],44100,1,200,-24)||
   spmix_attach(m,1,ringdata,&phase[1],22050,1,120,24))return 1;
 start=clock();
 for(k=0;k<5000;k++)spmix_render(m,out,1024);
 end=clock();sec=(double)(end-start)/CLOCKS_PER_SEC;
 frames_per_s=sec>0?5000*1024.0/sec:0;
 printf("HOST ONLY: output_rate=48000Hz, 2 mono inputs 44100+22050Hz, frames=%llu, cpu_seconds=%.3f, output_frames_per_cpu_second=%.0f, xRealtime=%.1f, struct_bytes=%zu, clipped=%u\n",
  (unsigned long long)m->frames_rendered,sec,frames_per_s,frames_per_s/48000.0,sizeof(*m),m->clipped_samples);
 free(m);return 0;
}
