#include "spmp_amr_opencore.h"
#include <string.h>
/* opencore-amrnb ABI, see OpenCORE's amrnb/interf_dec.h */
extern void *Decoder_Interface_init(void);
extern void Decoder_Interface_exit(void *);
extern void Decoder_Interface_Decode(void *,const unsigned char *,short *,int);
/* opencore-amrwb ABI, see OpenCORE's amrwb/dec_if.h */
extern void *D_IF_init(void);
extern void D_IF_exit(void *);
extern void D_IF_decode(void *,const unsigned char *,short *,int);
static const unsigned char nbbytes[16]={13,14,16,18,20,21,27,32,6,0,0,0,0,0,0,1};
static const unsigned char wbbytes[16]={18,24,33,37,41,47,51,59,61,6,0,0,0,0,0,1};
static int rd(sp_amr_opencore_ctx*s,void*dst,unsigned bytes){unsigned done=0;while(done<bytes){int n=s->io.read(s->io.user,(char*)dst+done,bytes-done);if(n<=0)return -1;if((unsigned)n>bytes-done)return -1;done+=(unsigned)n;}return 0;}
static int sk(sp_amr_opencore_ctx*s,uint64_t offset){return s->io.seek&&s->io.seek(s->io.user,offset)==0?0:-1;}
static int amr_open(void*ctx,sp_audio_io io,void*ws,size_t wn,sp_audio_info*out){
 sp_amr_opencore_ctx*s=ctx;unsigned hdr;char magic[9];(void)ws;(void)wn;
 if(!s||!out||!io.read)return SP_AUD_BAD;
 s->io=io;s->sample_count=s->sample_offset=0;s->frame_index=0;s->at_eof=0;
 hdr=s->type==SP_FMT_AMR_NB?6:9;
 if(rd(s,magic,hdr)||memcmp(magic,s->type==SP_FMT_AMR_NB?"#!AMR\n":"#!AMR-WB\n",hdr))return SP_AUD_BAD;
 s->decoder=s->type==SP_FMT_AMR_NB?Decoder_Interface_init():D_IF_init();
 if(!s->decoder)return SP_AUD_NO_MEMORY;
 out->format=(uint16_t)s->type;out->codec=(uint16_t)s->type;
 out->rate=s->type==SP_FMT_AMR_NB?8000:16000;out->channels=1;out->bits=16;
 return 0;
}
static int amr_decode(void*ctx,int16_t*pcm,size_t cap,size_t*written){
 sp_amr_opencore_ctx*s=ctx;size_t made=0;unsigned frame_samples=s->type==SP_FMT_AMR_NB?160:320;
 if(written)*written=0;if(!pcm||!written||!cap||!s||!s->decoder)return SP_AUD_BAD;
 while(made<cap){if(s->sample_offset<s->sample_count){size_t n=s->sample_count-s->sample_offset;
   if(n>cap-made)n=cap-made;
   memcpy(pcm+made,s->samples+s->sample_offset,n*sizeof(int16_t));
   s->sample_offset+=(unsigned)n;made+=n;continue;
  }
  if(s->at_eof)break;
  {unsigned char packet[64];unsigned type,len;int n=s->io.read(s->io.user,packet,1);
   if(n==0){s->at_eof=1;break;}if(n!=1)return SP_AUD_IO;
   type=(packet[0]>>3)&15;
   len=s->type==SP_FMT_AMR_NB?nbbytes[type]:wbbytes[type];
   if(!len)return SP_AUD_UNSUPPORTED;
   if(len>1 && rd(s,packet+1,len-1))return SP_AUD_BAD;
   if(s->type==SP_FMT_AMR_NB)Decoder_Interface_Decode(s->decoder,packet,s->samples,0);
   else D_IF_decode(s->decoder,packet,s->samples,0);
   s->sample_count=frame_samples;s->sample_offset=0;s->frame_index++;
  }
 }
 *written=made;return made?SP_AUD_OK:SP_AUD_EOF;
}
static int amr_seek(void*ctx,uint64_t ms){
 sp_amr_opencore_ctx*s=ctx;uint64_t frames=ms/20,k;
 unsigned hdr=s->type==SP_FMT_AMR_NB?6:9;
 if(!s->io.seek)return SP_AUD_UNSUPPORTED;
 if(sk(s,hdr))return SP_AUD_IO;
 if(s->decoder){if(s->type==SP_FMT_AMR_NB){Decoder_Interface_exit(s->decoder);s->decoder=Decoder_Interface_init();}
 else{D_IF_exit(s->decoder);s->decoder=D_IF_init();}}
 if(!s->decoder)return SP_AUD_NO_MEMORY;
 s->at_eof=0;s->sample_count=s->sample_offset=0;s->frame_index=0;
 for(k=0;k<frames;k++){unsigned char toc;unsigned typ,len;
  int r=s->io.read(s->io.user,&toc,1);if(r==0){s->at_eof=1;return SP_AUD_OK;}
  if(r!=1)return SP_AUD_IO;
  typ=(toc>>3)&15;len=s->type==SP_FMT_AMR_NB?nbbytes[typ]:wbbytes[typ];if(!len)return SP_AUD_UNSUPPORTED;
  if(len>1){unsigned char temp[64];if(rd(s,temp,len-1))return SP_AUD_BAD;}
  s->frame_index++;
 }
 return 0;
}
static void amr_close(void*ctx){sp_amr_opencore_ctx*s=ctx;if(!s||!s->decoder)return;
 if(s->type==SP_FMT_AMR_NB)Decoder_Interface_exit(s->decoder);else D_IF_exit(s->decoder);
 s->decoder=0;
}
void sp_amr_opencore_make(sp_audio_external_backend*out,sp_amr_opencore_ctx*s,int fmt){
 if(!out||!s)return;memset(out,0,sizeof(*out));memset(s,0,sizeof(*s));s->type=fmt;
 out->ctx=s;out->open=amr_open;out->decode=amr_decode;out->seek_ms=amr_seek;out->close=amr_close;
}
