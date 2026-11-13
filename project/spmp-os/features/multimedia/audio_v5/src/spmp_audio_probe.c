#include "spmp_audio_v5.h"
#include <string.h>
#include <stdint.h>
static uint16_t le16(const uint8_t*p){return (uint16_t)(p[0]|p[1]<<8);}
static uint32_t le32(const uint8_t*p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint64_t le64(const uint8_t*p){return (uint64_t)le32(p)|((uint64_t)le32(p+4)<<32);}
static const uint8_t asf_file_props[16]={0xa1,0xdc,0xab,0x8c,0x47,0xa9,0xcf,0x11,0x8e,0xe4,0x00,0xc0,0x0c,0x20,0x53,0x65};
static const uint8_t asf_stream_props[16]={0x91,0x07,0xdc,0xb7,0xb7,0xa9,0xcf,0x11,0x8e,0xe6,0x00,0xc0,0x0c,0x20,0x53,0x65};
static const uint8_t asf_audio_stream[16]={0x40,0x9e,0x69,0xf8,0x4d,0x5b,0xcf,0x11,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b};
static const uint8_t asf_head_guid[16]={0x30,0x26,0xb2,0x75,0x8e,0x66,0xcf,0x11,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c};
int sp_audio_probe(sp_audio_io io,int fmt,sp_audio_info*out){uint8_t b[4096];size_t pos=0;int n;if(!io.read||!out)return SP_AUD_BAD;memset(out,0,sizeof(*out));
 if(fmt==SP_FMT_AUTO){n=io.read(io.user,b,16);if(n<16)return SP_AUD_BAD;fmt=sp_audio_detect(b,16);if(!io.seek||io.seek(io.user,0))return SP_AUD_IO;}
 out->format=fmt;
 if(fmt==SP_FMT_AMR_NB||fmt==SP_FMT_AMR_WB){static const uint8_t nb[]={13,14,16,18,20,21,27,32,6,0,0,0,0,0,0,1};
  static const uint8_t wb[]={18,24,33,37,41,47,51,59,61,6,0,0,0,0,0,1};
  uint8_t header[9],packet[64];const uint8_t*table=fmt==SP_FMT_AMR_NB?nb:wb;
  unsigned hdr=fmt==SP_FMT_AMR_NB?6:9;uint64_t frames=0;out->rate=fmt==SP_FMT_AMR_NB?8000:16000;out->channels=1;out->bits=16;
  {unsigned got=0;while(got<hdr){int r=io.read(io.user,header+got,hdr-got);if(r<=0|| (unsigned)r>hdr-got)return SP_AUD_BAD;got+=(unsigned)r;}
   if(sp_audio_detect(header,hdr)!=fmt)return SP_AUD_BAD;}
  while(frames<100000000){unsigned type,total;int i=io.read(io.user,packet,1);if(i==0)break;if(i!=1)return SP_AUD_IO;
   type=(packet[0]>>3)&15;total=table[type];if(!total)return SP_AUD_UNSUPPORTED;
   {unsigned got=1;while(got<total){int r=io.read(io.user,packet+got,total-got);
    if(r<=0 || (unsigned)r>total-got)return SP_AUD_BAD;got+=(unsigned)r;}}
   frames++;
  }
  out->total_frames=frames*(fmt==SP_FMT_AMR_NB?160:320);return 0;
 }
 if(fmt==SP_FMT_WMA_ASF){int got=0,got_duration=0;uint64_t duration_100ns=0,preroll_ms=0;
  n=0;while((size_t)n<sizeof(b)){int r=io.read(io.user,b+n,(unsigned)(sizeof(b)-(size_t)n));
    if(r<0||(size_t)r>sizeof(b)-(size_t)n)return SP_AUD_IO;
    if(r==0)break;n+=r;}
  if(n<30||memcmp(b,asf_head_guid,16))return SP_AUD_BAD;
  pos=30;while(pos+24<=(size_t)n){uint64_t len=le64(b+pos+16);if(len<24||len>(uint64_t)n-pos)break;
   if(!memcmp(b+pos,asf_file_props,16)&&len>=104){duration_100ns=le64(b+pos+64);preroll_ms=le64(b+pos+80);got_duration=1;}
   if(!memcmp(b+pos,asf_stream_props,16)&&len>=100){size_t off=pos+24;
    if(!memcmp(b+off,asf_audio_stream,16)){size_t data=off+54;
     if(data+16<=pos+len){out->codec=le16(b+data);out->channels=le16(b+data+2);out->rate=le32(b+data+4);out->bitrate=le32(b+data+8)*8;got=1;}
    }
   }
   pos+=(size_t)len;
  }
  if(got_duration && out->rate){uint64_t usable=duration_100ns;
    if(preroll_ms<36000000u && duration_100ns>preroll_ms*10000u)usable-=preroll_ms*10000u;
    out->total_frames=usable/10000000u*out->rate + (usable%10000000u)*out->rate/10000000u;
  }
  return got?0:SP_AUD_UNSUPPORTED;
 }
 return SP_AUD_UNSUPPORTED;
}
