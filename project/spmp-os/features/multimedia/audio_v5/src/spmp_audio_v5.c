#include "spmp_audio_v5.h"
#include "spmp_flac_lite.h"
#include "spmp_ima_adpcm.h"
#include "ivorbisfile.h"
#include <string.h>
#include <stdio.h>
#define FB (128u*1024u)
#define FW (64u*1024u)
struct sp_audio_player {
 sp_audio_io io;sp_audio_info info;
 uint64_t off,data_begin,data_size,data_used;
 int format,ogg_open,flac_eof,backend_open;
 sp_audio_external_backend backend;
 uint16_t wav_align,wav_depth,wav_ext_frames;
 size_t ima_off,ima_len; int16_t *ima_pcm; size_t ima_capacity;
 uint8_t wavebuf[8192];
 uint8_t *in,*scratch;size_t in_size,in_off;
 spmp_flac flac;
 OggVorbis_File ov;
};
static uint16_t le16(const uint8_t*p){return (uint16_t)(p[0]|(p[1]<<8));}
static uint32_t le32(const uint8_t*p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint32_t be24(const uint8_t*p){return (uint32_t)p[0]<<16|(uint32_t)p[1]<<8|p[2];}
static int read_all(sp_audio_player *s,void*out,size_t n){uint8_t*p=out;while(n){int k=s->io.read(s->io.user,p,(unsigned)(n>16384?16384:n));if(k<=0)return SP_AUD_IO;n-=k;p+=k;s->off+=k;}return 0;}
static int go(sp_audio_player*s,uint64_t pos){if(!s->io.seek||s->io.seek(s->io.user,pos))return SP_AUD_IO;s->off=pos;return 0;}
static int skip(sp_audio_player*s,uint64_t n){uint8_t temp[256];if(s->io.seek)return go(s,s->off+n);while(n){size_t x=n>sizeof(temp)?sizeof(temp):(size_t)n;if(read_all(s,temp,x))return SP_AUD_IO;n-=x;}return 0;}
size_t sp_audio_player_size(void){return sizeof(sp_audio_player);}
int sp_audio_detect(const uint8_t*p,size_t n){if(!p)return SP_FMT_AUTO;
 if(n>=12&&!memcmp(p,"RIFF",4)&&!memcmp(p+8,"WAVE",4))return SP_FMT_WAV;
 if(n>=4&&!memcmp(p,"fLaC",4))return SP_FMT_FLAC;
 if(n>=4&&!memcmp(p,"OggS",4))return SP_FMT_OGG_VORBIS;
 if(n>=9&&!memcmp(p,"#!AMR-WB\n",9))return SP_FMT_AMR_WB;
 if(n>=6&&!memcmp(p,"#!AMR\n",6))return SP_FMT_AMR_NB;
 {static const uint8_t g[16]={0x30,0x26,0xb2,0x75,0x8e,0x66,0xcf,0x11,0xa6,0xd9,0,0xaa,0,0x62,0xce,0x6c};if(n>=16&&!memcmp(p,g,16))return SP_FMT_WMA_ASF;}
 return SP_FMT_AUTO;}
static int open_wav(sp_audio_player*s){uint8_t h[12],c[8],v[40]={0};int fmt=0;unsigned i;
 if(read_all(s,h,12)||memcmp(h,"RIFF",4)||memcmp(h+8,"WAVE",4))return SP_AUD_BAD;
 for(i=0;i<256;i++){uint32_t n;uint64_t start;
  if(read_all(s,c,8))return SP_AUD_BAD;n=le32(c+4);start=s->off;
  if(!memcmp(c,"fmt ",4)){
   if(n<16||n>65536)return SP_AUD_BAD;
   if(read_all(s,v,16))return SP_AUD_BAD;
   s->info.codec=le16(v);s->info.channels=le16(v+2);s->info.rate=le32(v+4);
   s->info.bitrate=le32(v+8)*8;s->wav_align=le16(v+12);s->wav_depth=le16(v+14);
   if(s->info.codec==0xfffe&&n>=40){
    if(read_all(s,v+16,24))return SP_AUD_BAD;
    /* SubFormat GUID: XXXXXXXX-0000-0010-8000-00AA00389B71 */
    if(v[26]||v[27]||v[28]||v[29]||v[30]!=0x10||v[31]||v[32]!=0x80||v[33]||v[34]||v[35]!=0xaa||v[36]||v[37]!=0x38||v[38]!=0x9b||v[39]!=0x71)return SP_AUD_UNSUPPORTED;
    if(le16(v+24)!=0)return SP_AUD_UNSUPPORTED;
    s->info.codec=le16(v+24);
   }else if(s->info.codec==0x11&&n>=20){if(read_all(s,v+16,4))return SP_AUD_BAD;s->wav_ext_frames=le16(v+18);}
   fmt=1;
  }else if(!memcmp(c,"data",4)){s->data_begin=start;s->data_size=n;break;}
  if(s->off>start+n||skip(s,start+n+(n&1)-s->off))return SP_AUD_IO;
 }
 if(!fmt||!s->data_begin||s->info.channels<1||s->info.channels>2||s->info.rate<1000||s->info.rate>192000)return SP_AUD_UNSUPPORTED;
 if(s->info.codec==0x11){size_t block_frames;
  if(s->wav_depth!=4||s->wav_align<4*s->info.channels||s->wav_align>sizeof(s->wavebuf)||
     s->wav_align% (4*s->info.channels)!=0)return SP_AUD_UNSUPPORTED;
  block_frames=1+((s->wav_align-4*s->info.channels)*2/s->info.channels);
  if(s->wav_ext_frames && s->wav_ext_frames!=block_frames)return SP_AUD_BAD;
  if(s->data_size%s->wav_align)return SP_AUD_UNSUPPORTED;
  if(!s->ima_pcm||s->ima_capacity<block_frames*s->info.channels)return SP_AUD_NO_MEMORY;
  s->info.bits=16;s->info.total_frames=(s->data_size/s->wav_align)*block_frames;
  return 0;
 }
 if(s->info.codec!=1)return SP_AUD_UNSUPPORTED;
 if(s->wav_depth!=8&&s->wav_depth!=16&&s->wav_depth!=24&&s->wav_depth!=32)return SP_AUD_UNSUPPORTED;
 if(s->wav_align!=s->info.channels*(s->wav_depth/8))return SP_AUD_BAD;
 s->info.bits=16;s->info.total_frames=s->data_size/s->wav_align;return 0;}
static int open_flac(sp_audio_player*s,void*workspace,size_t wn){size_t n=0,first=0;
 if(wn<FB+FW||!workspace)return SP_AUD_NO_MEMORY;
 s->in=workspace;s->scratch=s->in+FB;
 while(n<FB){int r=s->io.read(s->io.user,s->in+n,(unsigned)(FB-n>4096?4096:FB-n));if(r<=0)break;n+=(unsigned)r;s->off+=(unsigned)r;
  if(n>=8&&!memcmp(s->in,"fLaC",4)){
   size_t at=4;while(at+4<=n){uint32_t size=be24(s->in+at+1);int last=s->in[at]&0x80;
    if(size>FB||at+4+size>n)break;at+=4+size;if(last){first=at;break;}}
   if(first&&n>=first+16)break;
  }
 }
 if(!first||n<first+16)return SP_AUD_BAD;
 if(spmp_flac_open(&s->flac,s->in,n)!=0)return SP_AUD_UNSUPPORTED;
 if(spmp_flac_work_size(s->flac.max_block)>FW)return SP_AUD_NO_MEMORY;
 s->in_size=n;s->in_off=first;s->data_begin=first;
 s->info.rate=s->flac.sample_rate;s->info.channels=s->flac.channels;
 s->info.total_frames=s->flac.total_samples;s->info.bits=16;return 0;}
static size_t ogg_read(void*ptr,size_t size,size_t nmemb,void*ctx){sp_audio_player*s=ctx;int r;
 if(!size||nmemb>UINT32_MAX/size)return 0;
 r=s->io.read(s->io.user,ptr,(unsigned)(nmemb*size));if(r<=0)return 0;s->off+=r;return (size_t)r/size;}
static int ogg_seek(void*ctx,ogg_int64_t delta,int where){sp_audio_player*s=ctx;int64_t to;
 if(where==SEEK_SET)to=delta;
 else if(where==SEEK_CUR){if(s->off>INT64_MAX)return -1;to=(int64_t)s->off+delta;}
 else if(where==SEEK_END){int64_t length=s->io.size?s->io.size(s->io.user):-1;if(length<0)return -1;to=length+delta;}
 else return -1;
 return to<0||go(s,(uint64_t)to)!=0?-1:0;}
static long ogg_tell(void*ctx){sp_audio_player*s=ctx;return s->off>INT32_MAX?-1:(long)s->off;}
static int ogg_close(void*ctx){(void)ctx;return 0;}
static int open_ogg(sp_audio_player*s){ov_callbacks cb;vorbis_info*v;
 cb.read_func=ogg_read;cb.seek_func=s->io.seek?ogg_seek:0;cb.close_func=ogg_close;cb.tell_func=ogg_tell;
 if(ov_open_callbacks(s,&s->ov,NULL,0,cb)<0)return SP_AUD_BAD;
 s->ogg_open=1;v=ov_info(&s->ov,-1);if(!v||v->channels<1||v->channels>2||v->rate<1000||v->rate>192000)return SP_AUD_UNSUPPORTED;
 s->info.rate=v->rate;s->info.channels=v->channels;s->info.bits=16;
 if(ov_seekable(&s->ov)){ogg_int64_t total=ov_pcm_total(&s->ov,-1);if(total>0)s->info.total_frames=(uint64_t)total;}
 return 0;}
int sp_audio_open_with_backend(sp_audio_player*s,sp_audio_io io,int fmt,void*work,size_t wn,const sp_audio_external_backend *backend){int r;uint8_t h[16];
 if(!s||!io.read||!work)return SP_AUD_BAD;
 memset(s,0,sizeof(*s));s->io=io;
 if(!fmt){if(read_all(s,h,16))return SP_AUD_BAD;fmt=sp_audio_detect(h,16);if(go(s,0))return SP_AUD_IO;}
 s->format=fmt;s->info.format=(uint16_t)fmt;
 if(fmt==SP_FMT_WAV){s->ima_pcm=(int16_t*)work;s->ima_capacity=wn/sizeof(int16_t);r=open_wav(s);}
 else if(fmt==SP_FMT_FLAC)r=open_flac(s,work,wn);
 else if(fmt==SP_FMT_OGG_VORBIS)r=open_ogg(s);
 else if((fmt==SP_FMT_WMA_ASF||fmt==SP_FMT_AMR_NB||fmt==SP_FMT_AMR_WB)&&backend && backend->open && backend->decode){
    s->backend=*backend;r=backend->open(backend->ctx,io,work,wn,&s->info);
    if(r==0)s->backend_open=1;
 }else r=SP_AUD_NEED_BACKEND;
 if(r)sp_audio_close(s);return r;}
int sp_audio_open(sp_audio_player*s,sp_audio_io io,int fmt,void*work,size_t wn){
 return sp_audio_open_with_backend(s,io,fmt,work,wn,0);
}
static int16_t wave_sample(const uint8_t*p,int bytes){int32_t v;
 if(bytes==1)return (int16_t)(((int)p[0]-128)*256);
 if(bytes==2)return (int16_t)le16(p);
 if(bytes==3){v=(int32_t)(p[0]|p[1]<<8|p[2]<<16);if(v&0x800000)v|=(int32_t)0xff000000;return (int16_t)(v>>8);}
 v=(int32_t)le32(p);return (int16_t)(v>>16);}
int sp_audio_decode(sp_audio_player*s,int16_t*out,size_t cap,size_t*written){if(written)*written=0;if(!s||!out||!cap||!written)return SP_AUD_BAD;
 if(s->backend_open)return s->backend.decode(s->backend.ctx,out,cap,written);
 if(s->format==SP_FMT_WAV && s->info.codec==0x11){size_t frames;
  if(s->ima_off==s->ima_len){int decoded;
   if(s->data_used>=s->data_size)return SP_AUD_EOF;
   if(read_all(s,s->wavebuf,s->wav_align))return SP_AUD_IO;
   s->data_used+=s->wav_align;
   decoded=sp_ima_decode_block(s->wavebuf,s->wav_align,s->info.channels,s->ima_pcm,s->ima_capacity);
   if(decoded<=0)return SP_AUD_BAD;s->ima_len=(size_t)decoded;s->ima_off=0;
  }
  frames=s->ima_len-s->ima_off;if(frames>cap)frames=cap;
  memcpy(out,s->ima_pcm+s->ima_off*s->info.channels,frames*s->info.channels*sizeof(int16_t));
  s->ima_off+=frames;*written=frames;return 0;
 }
 if(s->format==SP_FMT_WAV){size_t frames=(size_t)((s->data_size-s->data_used)/s->wav_align),bytes,j;
  if(frames>cap)frames=cap;if(frames>sizeof(s->wavebuf)/s->wav_align)frames=sizeof(s->wavebuf)/s->wav_align;
  if(!frames)return SP_AUD_EOF;
  bytes=frames*s->wav_align;if(read_all(s,s->wavebuf,bytes))return SP_AUD_IO;
  for(j=0;j<frames*s->info.channels;j++)out[j]=wave_sample(s->wavebuf+j*s->wav_depth/8,s->wav_depth/8);
  s->data_used+=bytes;*written=frames;return SP_AUD_OK;}
 if(s->format==SP_FMT_OGG_VORBIS){int bitstream=0;long bytes;
  if(cap>INT32_MAX/(s->info.channels*2))cap=INT32_MAX/(s->info.channels*2);
  bytes=ov_read(&s->ov,(char*)out,(int)(cap*s->info.channels*2),&bitstream);
  if(bytes<0)return SP_AUD_BAD;if(bytes==0)return SP_AUD_EOF;
  *written=(size_t)bytes/(2*s->info.channels);return *written?0:SP_AUD_BAD;}
 if(s->format==SP_FMT_FLAC){size_t used=0,n=0;spmp_flac m=s->flac;int rc;
  if(cap<s->flac.max_block)return SP_AUD_NO_MEMORY;
  for(;;){rc=spmp_flac_decode_chunk(&m,s->in+s->in_off,s->in_size-s->in_off,s->scratch,FW,out,cap*s->info.channels,&used,&n);
   if(rc==SPFL_OK){s->in_off+=used;*written=n;return 0;}
   if(s->in_off==s->in_size && s->flac_eof)return SP_AUD_EOF;
   if(s->flac_eof)return SP_AUD_BAD;
   if(s->in_off){memmove(s->in,s->in+s->in_off,s->in_size-s->in_off);s->in_size-=s->in_off;s->in_off=0;}
   if(s->in_size>=FB)return SP_AUD_NO_MEMORY;
   {int r=s->io.read(s->io.user,s->in+s->in_size,(unsigned)(FB-s->in_size>8192?8192:FB-s->in_size));
    if(r<0)return SP_AUD_IO;if(!r)s->flac_eof=1;else{s->in_size+=r;s->off+=r;}}
  }}return SP_AUD_NEED_BACKEND;}
int sp_audio_seek_ms(sp_audio_player*s,uint64_t ms){if(!s)return SP_AUD_BAD;
 if(s->backend_open)return s->backend.seek_ms?s->backend.seek_ms(s->backend.ctx,ms):SP_AUD_UNSUPPORTED;
 if(s->format==SP_FMT_WAV && s->info.codec==0x11){uint64_t target=ms*s->info.rate/1000;
  uint64_t fpb=1+((s->wav_align-4*s->info.channels)*2/s->info.channels),block;
  if(target>s->info.total_frames)target=s->info.total_frames;
  block=target/fpb;if(go(s,s->data_begin+block*s->wav_align))return SP_AUD_IO;
  s->data_used=block*s->wav_align;s->ima_len=s->ima_off=0;
  if(target%fpb && s->data_used<s->data_size){int decoded;
    if(read_all(s,s->wavebuf,s->wav_align))return SP_AUD_IO;s->data_used+=s->wav_align;
    decoded=sp_ima_decode_block(s->wavebuf,s->wav_align,s->info.channels,s->ima_pcm,s->ima_capacity);
    if(decoded<=0)return SP_AUD_BAD;s->ima_len=(size_t)decoded;s->ima_off=(size_t)(target%fpb);
  }return 0;
 }
 if(s->format==SP_FMT_WAV){uint64_t frame=ms*s->info.rate/1000;
  if(frame>s->info.total_frames)frame=s->info.total_frames;
  if(go(s,s->data_begin+frame*s->wav_align))return SP_AUD_IO;s->data_used=frame*s->wav_align;return 0;}
 if(s->format==SP_FMT_OGG_VORBIS){if(!ov_seekable(&s->ov))return SP_AUD_UNSUPPORTED;
  return ov_pcm_seek(&s->ov,(ogg_int64_t)(ms*s->info.rate/1000))==0?0:SP_AUD_IO;}
 if(s->format==SP_FMT_FLAC)return SP_AUD_UNSUPPORTED;
 return SP_AUD_NEED_BACKEND;}
void sp_audio_close(sp_audio_player*s){if(!s)return;
 if(s->ogg_open){ov_clear(&s->ov);s->ogg_open=0;}
 if(s->backend_open){if(s->backend.close)s->backend.close(s->backend.ctx);s->backend_open=0;}
}
sp_audio_info sp_audio_get_info(const sp_audio_player*s){sp_audio_info r={0};if(s)r=s->info;return r;}
