#include "spmp_wma_asf_v6.h"
#include <string.h>
/* GUIDs are on-disk little endian. */
static const uint8_t g_hdr[16]={0x30,0x26,0xb2,0x75,0x8e,0x66,0xcf,0x11,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c};
static const uint8_t g_file[16]={0xa1,0xdc,0xab,0x8c,0x47,0xa9,0xcf,0x11,0x8e,0xe4,0x00,0xc0,0x0c,0x20,0x53,0x65};
static const uint8_t g_stream[16]={0x91,0x07,0xdc,0xb7,0xb7,0xa9,0xcf,0x11,0x8e,0xe6,0x00,0xc0,0x0c,0x20,0x53,0x65};
static const uint8_t g_audio[16]={0x40,0x9e,0x69,0xf8,0x4d,0x5b,0xcf,0x11,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b};
static const uint8_t g_data[16]={0x36,0x26,0xb2,0x75,0x8e,0x66,0xcf,0x11,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c};
static uint16_t u16(const uint8_t*b){return (uint16_t)((uint16_t)b[0]|((uint16_t)b[1]<<8));}
static uint32_t u32(const uint8_t*b){return (uint32_t)b[0]|((uint32_t)b[1]<<8)|((uint32_t)b[2]<<16)|((uint32_t)b[3]<<24);}
static uint64_t u64(const uint8_t*b){return (uint64_t)u32(b)|((uint64_t)u32(b+4)<<32);}
static int exact(spwma_io io,void*p,uint32_t n){
 uint32_t done=0;while(done<n){int r=io.read(io.user,(uint8_t*)p+done,n-done);
  if(r<0 || (uint32_t)r>n-done)return -3;
  if(r==0)return -1;
  done+=(uint32_t)r;
 }
 return 0;
}
static int at(spwma_io io,uint64_t offset,void*p,uint32_t len){
 if(io.seek(io.user,offset)!=0)return -3;
 return exact(io,p,len);
}
int spwma_probe(spwma_io io,spwma_info *out){
 uint8_t head[30],obj[24],wfx[18],prop[48],fmt[54];
 uint64_t limit,pos;uint32_t count,i;
 int valid_stream=0,valid_file=0,rc;int64_t file_size=-1;
 spwma_info result;
 if(!out||!io.read||!io.seek)return -1;
 memset(&result,0,sizeof(result));
 if(io.size)file_size=io.size(io.user);
 rc=at(io,0,head,sizeof(head));if(rc)return rc;
 if(memcmp(head,g_hdr,16)!=0)return -1;
 limit=u64(head+16);count=u32(head+24);
 if(limit<30||limit>(16u*1024u*1024u)||count>4096||
    (file_size>=0 && limit>(uint64_t)file_size))return -1;
 result.header_bytes=limit;pos=30;
 for(i=0;i<count;i++){
  uint64_t length,end;
  if(pos>limit || limit-pos<sizeof(obj))return -1;
  rc=at(io,pos,obj,sizeof(obj));if(rc)return rc;
  length=u64(obj+16);
  if(length<24 || length>limit-pos)return -1;
  end=pos+length;
  if(!memcmp(obj,g_file,16)){
   if(length<104)return -1;
   rc=at(io,pos+56,prop,sizeof(prop));if(rc)return rc;
   result.packets=u64(prop+0); /* file props: data packets at object offset 56 */
   result.preroll_ms=u64(prop+24);
   result.packet_size=u32(prop+40);
   /* playDuration in 100ns, subtract preroll milliseconds safely */
   {
    uint64_t d=u64(prop+8)/10000u;
    result.duration_ms=d>result.preroll_ms?d-result.preroll_ms:0;
   }
   if(result.packet_size==0)return -1;
   valid_file=1;
  }else if(!memcmp(obj,g_stream,16)){
   uint32_t databytes;
   if(length<96)return -1;
   rc=at(io,pos+24,fmt,sizeof(fmt));if(rc)return rc;
   if(!memcmp(fmt,g_audio,16)){
    databytes=u32(fmt+40); /* stream props type-specific size at offset 64 */
    if(databytes<18 || (uint64_t)databytes>length-78u)return -1;
    rc=at(io,pos+78,wfx,sizeof(wfx));if(rc)return rc;
    if(!valid_stream){
     result.codec_id=u16(wfx);
     result.channels=u16(wfx+2);
     result.sample_rate=u32(wfx+4);
     result.bitrate_bps=u32(wfx+8)>0x1fffffffu?0xffffffffu:u32(wfx+8)*8u;
     result.block_align=u16(wfx+12);
     result.bits_per_sample=u16(wfx+14);
     result.stream_number=u16(fmt+48)&0x7fu;
     if(result.channels<1||result.channels>2||result.sample_rate<8000||result.sample_rate>192000||!result.block_align)return -1;
     valid_stream=1;
    }
   }
  }
  pos=end;
 }
 if(pos!=limit || !valid_stream || !valid_file)return -1;
 if(file_size<0 || (uint64_t)file_size>=limit+50u){
  uint8_t dh[24];rc=at(io,limit,dh,sizeof(dh));
  if(!rc && !memcmp(dh,g_data,16) && u64(dh+16)>=50u)result.data_offset=limit+50u;
 }
 *out=result;
 return (result.codec_id==0x160u||result.codec_id==0x161u)?0:-2;
}
