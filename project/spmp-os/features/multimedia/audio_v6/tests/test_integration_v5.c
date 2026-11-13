/* Foreground end-to-end: Audio v5 decoder -> Audio v6 mixer -> stereo PCM.
 * AMR-NB/WB use OpenCORE installed on the host; not proof of ARM decode. */
#include "spmp_mixer_v6.h"
#include "spmp_mix_decoders_v6.h"
#include "spmp_mix_audio_v5_bridge.h"
#include "spmp_audio_v5.h"
#include "spmp_amr_opencore.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
static int r(void*u,void*p,unsigned n){FILE*f=u;int got=(int)fread(p,1,n,f);return got?got:ferror(f)?-1:0;}
static int sk(void*u,uint64_t pos){return pos>0x7fffffffu?-1:fseek(u,(long)pos,SEEK_SET);}
static int64_t sz(void*u){FILE*f=u;long at=ftell(f),len;if(at<0||fseek(f,0,SEEK_END))return -1;len=ftell(f);fseek(f,at,SEEK_SET);return len;}
int main(int argc,char**argv){
 sp_audio_player *p=0;sp_audio_external_backend backend;sp_amr_opencore_ctx ctx;
 sp_audio_io io;sp_audio_info inf;spmix_mixer *m=0;int16_t buffer[2048*2];
 unsigned char *ws=0;int16_t *pcm_cache=0;spmix_audio_v5_buffered v5buf;FILE*in=0,*out=0;int rc,fmt=0,iter;uint64_t total=0;uint32_t out_rate;
 if(argc<4 || argc>5){fprintf(stderr,"usage: %s input output.s16 rate [AMR format 5|6]\n",argv[0]);return 1;}
 in=fopen(argv[1],"rb");out=fopen(argv[2],"wb");if(!in||!out){fprintf(stderr,"cannot open io\n");return 2;}
 out_rate=(uint32_t)atoi(argv[3]);if(argc==5)fmt=atoi(argv[4]);
 p=calloc(1,sp_audio_player_size());ws=calloc(1,256*1024);m=calloc(1,sizeof(*m));
 pcm_cache=calloc(2*8192,sizeof(int16_t));
 if(!p||!ws||!m||!pcm_cache)return 3;
 io.user=in;io.read=r;io.seek=sk;io.size=sz;
 if(fmt==SP_FMT_AMR_NB||fmt==SP_FMT_AMR_WB){
  sp_amr_opencore_make(&backend,&ctx,fmt);
  rc=sp_audio_open_with_backend(p,io,fmt,ws,256*1024,&backend);
 }else rc=sp_audio_open(p,io,SP_FMT_AUTO,ws,256*1024);
 if(rc){fprintf(stderr,"sp_audio_open rc=%d\n",rc);return 4;}
 inf=sp_audio_get_info(p);
 if(spmix_audio_v5_buffered_init(&v5buf,p,pcm_cache,8192,inf.channels)||
    spmix_init(m,out_rate)||spmix_attach(m,0,spmix_audio_v5_buffered_pull,&v5buf,inf.rate,inf.channels,256,0))return 5;
 for(iter=0;iter<50000;iter++){
  unsigned active=spmix_active_voices(m);
  if(!active)break;
  if(spmix_render(m,buffer,2048)!=2048)return 6;
  if(fwrite(buffer,2,4096,out)!=4096)return 7;
  total+=2048;
 }
 fprintf(stderr,"AudioV5 fmt=%u rate=%u channels=%u output_rate=%u frames=%llu loops=%d starved=%u errors=%u\n",
 (unsigned)inf.format,inf.rate,inf.channels,out_rate,(unsigned long long)total,iter,m->starved_reads,m->input_errors);
 if(iter==50000||m->input_errors||v5buf.decoder_errors||!total)return 8;
 sp_audio_close(p);free(p);free(ws);free(m);free(pcm_cache);fclose(in);fclose(out);return 0;
}
