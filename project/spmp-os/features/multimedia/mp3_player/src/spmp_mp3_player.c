/* SPMP-OS MP3 streaming player, GPL-2.0-or-later due to libmad linkage. */
#include "spmp_mp3_internal.h"
#include <string.h>

_Static_assert(sizeof(spmp_mp3_player) <= SPMP_MP3_CONTEXT_MAX_BYTES,
               "SPMP MP3 context exceeds 48-KiB reserved arena");
static void scan_xing(spmp_mp3_player *p);
static uint32_t be32(const uint8_t *v){return ((uint32_t)v[0]<<24)|((uint32_t)v[1]<<16)|((uint32_t)v[2]<<8)|v[3];}
static int id3_len(const uint8_t *v,uint32_t *len){
 if(memcmp(v,"ID3",3)!=0)return 0;
 if(v[3]<2||v[3]>4||(v[6]|v[7]|v[8]|v[9])&0x80)return -1;
 *len=10+((uint32_t)v[6]<<21)+((uint32_t)v[7]<<14)+((uint32_t)v[8]<<7)+v[9];
 if(v[5]&0x10)*len+=10;
 return *len<=16u*1024u*1024u?1:-1;
}
static void reset_decoder(spmp_mp3_player *p){
 if(p->initialized){mad_synth_finish(&p->synth);mad_frame_finish(&p->frame);mad_stream_finish(&p->stream);}
 mad_stream_init(&p->stream);mad_frame_init(&p->frame);mad_synth_init(&p->synth);
 p->stream.main_data=(uint8_t (*)[MAD_BUFFER_MDLEN])&p->main_data;
 p->frame.overlap=(mad_fixed_t (*)[2][32][18])&p->overlap;
 memset(p->main_data,0,sizeof(p->main_data));memset(p->overlap,0,sizeof(p->overlap));
 p->valid=0;p->eof=0;p->finished=0;p->initialized=1;
}
size_t spmp_mp3_player_size(void){return sizeof(spmp_mp3_player);}
static int read_prefix(spmp_mp3_player *p){
 uint8_t h[10];size_t n=0;int z;uint32_t skip=0;int tag;
 while(n<10){z=p->io.read(p->io.ctx,h+n,10-n);if(z<0||z>10-(int)n)return -1;if(z==0)break;n+=(size_t)z;}
 if(n<10)return -1;
 tag=id3_len(h,&skip);if(tag<0)return -1;
 if(tag==1){uint8_t tmp[512];size_t left=skip-10,take;
   while(left){take=left<sizeof(tmp)?left:sizeof(tmp);z=p->io.read(p->io.ctx,tmp,take);if(z<=0||(size_t)z>take)return -1;left-=(size_t)z;}
   p->base_offset=skip;p->next_offset=skip;
 }else{
   memcpy(p->data,h,10);p->valid=10;p->base_offset=0;p->next_offset=10;
 }
 return 0;
}
int spmp_mp3_open(spmp_mp3_player *p,const spmp_mp3_io *io,spmp_mp3_pcm_callback on_pcm,void *ctx){
 if(!p||!io||!io->read)return -1;
 memset(p,0,sizeof(*p));p->io=*io;p->on_pcm=on_pcm;p->pcm_ctx=ctx;p->volume=256;p->first_frame=1;
 if(io->seek && spmp_mp3_read_metadata(io,&p->meta)==0){
  p->base_offset=p->meta.audio_offset;p->next_offset=p->base_offset;
  if(io->seek(io->ctx,p->base_offset)!=0)return -1;
  reset_decoder(p);
 }else{
  if(io->seek && io->seek(io->ctx,0)!=0)return -1;
  reset_decoder(p);
  if(read_prefix(p)!=0)return -1;
  p->meta.audio_offset=p->base_offset;
  if(p->valid)mad_stream_buffer(&p->stream,p->data,(unsigned long)p->valid);
 }
 return 0;
}
static int refill(spmp_mp3_player *p){
 size_t remain=0;const unsigned char *ptr=0;int z;
 if(p->stream.buffer){
  ptr=p->stream.next_frame;
  if(p->stream.error==MAD_ERROR_BUFLEN && p->stream.this_frame)ptr=p->stream.this_frame;
  if(ptr && ptr>=p->stream.buffer && ptr<=p->stream.bufend){
   remain=(size_t)(p->stream.bufend-ptr);
   if(p->eof){if(remain<=MAD_BUFFER_GUARD)remain=0;else remain-=MAD_BUFFER_GUARD;}
   if(remain>SPMP_MP3_IOBUF)return -1;
   p->base_offset+=(uint64_t)(ptr-p->stream.buffer);
   memmove(p->data,ptr,remain);
  }
 }
 p->valid=remain;
 if(!p->eof){
  while(p->valid<SPMP_MP3_IOBUF){
   z=p->io.read(p->io.ctx,p->data+p->valid,SPMP_MP3_IOBUF-p->valid);
   if(z<0||(size_t)z>SPMP_MP3_IOBUF-p->valid)return -1;
   if(!z){p->eof=1;break;}
   p->valid+=(size_t)z;p->next_offset+=(uint64_t)z;
  }
 }
 if(p->eof)memset(p->data+p->valid,0,MAD_BUFFER_GUARD);
 if(!p->valid){p->finished=1;return 0;}
 if(p->first_frame && p->base_offset==p->meta.audio_offset)scan_xing(p);
 mad_stream_buffer(&p->stream,p->data,(unsigned long)p->valid+(p->eof?MAD_BUFFER_GUARD:0));
 return 1;
}
static unsigned bytes_per_frame_samples(const struct mad_header *h){
 if(h->layer==MAD_LAYER_I)return 384;
 if(h->layer==MAD_LAYER_II)return 1152;
 return (h->flags&MAD_FLAG_LSF_EXT)?576:1152;
}
/* Parse Xing/LAME from the raw first MP3 frame, including frames libmad skips.
 * Some encoders put a non-audio Info frame before the first playable frame. */
static void scan_xing(spmp_mp3_player *p){
 const uint8_t *f=p->data,*x;uint32_t h,flags;unsigned off,version,layer,channels,sridx,sr;
 static const unsigned rates[3]={44100,48000,32000};
 if(p->valid<160||p->base_offset!=p->meta.audio_offset)return;
 h=be32(f);version=(h>>19)&3;layer=(h>>17)&3;sridx=(h>>10)&3;channels=((h>>6)&3)==3?1:2;
 if((h&0xffe00000u)!=0xffe00000u||version==1||layer!=1||sridx==3)return;
 sr=rates[sridx];if(version==2)sr/=2;else if(version==0)sr/=4;
 off=4+(((h>>16)&1)?0:2)+((version==3)?(channels==1?17:32):(channels==1?9:17));
 if(off+8>p->valid)return;
 x=f+off;if(memcmp(x,"Xing",4)&&memcmp(x,"Info",4))return;
 p->meta.vbr=memcmp(x,"Xing",4)==0;
 flags=be32(x+4);off+=8;
 if(flags&1){if(off+4>p->valid)return;p->xing_frames=be32(f+off);off+=4;}
 if(flags&2){if(off+4>p->valid)return;p->xing_bytes=be32(f+off);off+=4;}
 if(flags&4){if(off+100>p->valid)return;memcpy(p->xing_toc,f+off,100);p->have_toc=1;off+=100;}
 if(flags&8){if(off+4>p->valid)return;off+=4;}
 if(p->xing_frames && off+24<=p->valid &&
    (!memcmp(f+off,"LAME",4)||!memcmp(f+off,"Lavc",4))) {
  unsigned delay=((unsigned)f[off+21]<<4)|(f[off+22]>>4);
  unsigned pad=((unsigned)(f[off+22]&15)<<8)|f[off+23];
  if(delay<4096 && pad>=529 && pad<4096){
    p->trim_start=(version==3?1152:576)+delay+529;p->trim_end=pad-529;
  }
 }
 if(p->xing_frames&&sr){
  /* Xing/Info's reported audio frame count excludes the Info frame. */
  uint64_t total=(uint64_t)(p->xing_frames+1u)*(version==3?1152u:576u);
  if(total>p->trim_start+p->trim_end)total-=p->trim_start+p->trim_end;
  p->duration_ms=(uint32_t)(total*1000u/sr);
  p->meta.duration_ms=p->duration_ms;
 }
}
static int16_t convert(mad_fixed_t s,unsigned gain){
 int32_t x;
 if(s>=MAD_F_ONE)s=MAD_F_ONE-1;
 else if(s < -MAD_F_ONE)s=-MAD_F_ONE;
 /* round to nearest 16-bit signed; do not overflow for gain<=256 */
 x=(s+(1L<<(MAD_F_FRACBITS-16)))>>(MAD_F_FRACBITS+1-16);
 x=(x*(int32_t)gain)>>8;
 if(x>32767)x=32767;if(x< -32768)x=-32768;
 return (int16_t)x;
}
int spmp_mp3_step(spmp_mp3_player *p){
 unsigned tries;
 if(!p||!p->initialized)return -1;
 if(p->paused)return 2;
 if(p->finished)return 0;
 if(!p->stream.buffer){int rc=refill(p);if(rc<=0)return rc;}
 for(tries=0;tries<4096;tries++){
  if(mad_frame_decode(&p->frame,&p->stream)==0){
   unsigned n,c,i,start=0,end;const mad_fixed_t *left,*right;
   mad_synth_frame(&p->synth,&p->frame);
   n=p->synth.pcm.length;c=p->synth.pcm.channels;
   if(n>SPMP_MP3_MAX_PCM_FRAMES || c<1 || c>2)return -1;
   if(p->first_frame){p->first_frame=0;p->meta.sample_rate=p->synth.pcm.samplerate;p->meta.channels=c;
    p->meta.bitrate_kbps=p->frame.header.bitrate/1000;
    if(!p->duration_ms && p->io.size && p->meta.bitrate_kbps){
     uint64_t sz=p->io.size(p->io.ctx);
     uint64_t ms=sz>p->meta.audio_offset?(sz-p->meta.audio_offset)*8/(uint64_t)p->meta.bitrate_kbps:0;
     p->duration_ms=ms>0xffffffffu?0xffffffffu:(uint32_t)ms;
    }
    p->meta.duration_ms=p->duration_ms;
   }
   left=p->synth.pcm.samples[0];right=p->synth.pcm.samples[c==2?1:0];
   end=n;
   if(p->raw_samples < p->trim_start){uint64_t x=p->trim_start-p->raw_samples;if(x>=n)start=n;else start=(unsigned)x;}
   if(p->xing_frames && p->trim_end){
    uint64_t limit=(uint64_t)(p->xing_frames+1u)*bytes_per_frame_samples(&p->frame.header)-p->trim_end;
    if(p->raw_samples>=limit)end=0;
    else if(p->raw_samples+n>limit)end=(unsigned)(limit-p->raw_samples);
   }
   if(end<start)end=start;
   if(p->meta.sample_rate){p->samples_total+=n;
    p->position_ms=(uint32_t)(p->output_samples*1000u/p->meta.sample_rate);}
   
   p->raw_samples+=n;p->valid_audio=1;
   if(end>start && p->suppress_until_ms && (p->samples_total*1000u/p->meta.sample_rate)<p->suppress_until_ms)start=end;
   for(i=start;i<end;i++){unsigned k=i-start;p->pcm[k*c]=convert(left[i],p->volume);
    if(c==2)p->pcm[2*k+1]=convert(right[i],p->volume);}
   p->output_samples+=end-start;
   if(p->meta.sample_rate) p->position_ms=(uint32_t)(p->output_samples*1000u/p->meta.sample_rate);
   if(end>start && p->on_pcm && p->on_pcm(p->pcm_ctx,p->pcm,end-start,p->synth.pcm.samplerate,c))return -2;
   return 1;
  }
  if(p->stream.error==MAD_ERROR_BUFLEN){int rc;
   if(p->eof){p->finished=1;return p->valid_audio?0:-1;}
   rc=refill(p);if(rc<=0)return rc;continue;
  }
  if(p->stream.error==MAD_ERROR_NOMEM)return -1;
  p->bad++;
  if(p->bad>200000u){p->finished=1;return -1;}
  if(!MAD_RECOVERABLE(p->stream.error))return -1;
 }
 return -1;
}
void spmp_mp3_pause(spmp_mp3_player *p,int v){if(p)p->paused=!!v;}
int spmp_mp3_paused(const spmp_mp3_player *p){return p?p->paused:0;}
void spmp_mp3_set_volume(spmp_mp3_player *p,unsigned v){if(p)p->volume=v>256?256:v;}
uint32_t spmp_mp3_position_ms(const spmp_mp3_player *p){return p?p->position_ms:0;}
const spmp_mp3_metadata *spmp_mp3_info(const spmp_mp3_player *p){return p?&p->meta:0;}
uint32_t spmp_mp3_bad_frames(const spmp_mp3_player *p){return p?p->bad:0;}
void spmp_mp3_close(spmp_mp3_player *p){if(p && p->initialized){mad_stream_finish(&p->stream);mad_frame_finish(&p->frame);mad_synth_finish(&p->synth);p->initialized=0;}}
int spmp_mp3_seek_ms(spmp_mp3_player *p,uint32_t ms){
 uint64_t length,byte_off,delta;uint32_t seek_ms;unsigned ix,frac;
 if(!p || !p->initialized || !p->io.seek || !p->io.size || !p->meta.sample_rate || !p->duration_ms)return -1;
 length=p->io.size(p->io.ctx);if(length<=p->meta.audio_offset)return -1;
 if(ms>p->duration_ms)ms=p->duration_ms;
 seek_ms=ms>200?ms-200:0;/* preroll to warm MP3 reservoir */
 if(p->have_toc && p->xing_bytes){
  uint64_t percent100=(uint64_t)seek_ms*10000u/p->duration_ms;
  if(percent100>9999)percent100=9999;
  ix=(unsigned)(percent100/100);frac=(unsigned)(percent100%100);
  delta=(uint64_t)(p->xing_toc[ix]*(100-frac)+(ix<99?p->xing_toc[ix+1]:256)*frac);
  byte_off=p->meta.audio_offset+(uint64_t)p->xing_bytes*delta/(25600u);
 }else{
  byte_off=p->meta.audio_offset+(length-p->meta.audio_offset)*(uint64_t)seek_ms/p->duration_ms;
 }
 if(byte_off>=length)byte_off=length-1;
 if(p->io.seek(p->io.ctx,byte_off)!=0)return -1;
 reset_decoder(p);p->base_offset=p->next_offset=byte_off;p->first_frame=0;
 p->samples_total=(uint64_t)seek_ms*p->meta.sample_rate/1000;p->position_ms=seek_ms;
 p->valid_audio=0;p->bad=0;p->raw_samples=0;p->output_samples=(uint64_t)seek_ms*p->meta.sample_rate/1000u;
 p->trim_start=p->trim_end=0;p->suppress_until_ms=ms;
 return 0;
}
