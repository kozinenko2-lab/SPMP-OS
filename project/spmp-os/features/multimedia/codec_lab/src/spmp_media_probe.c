#include "spmp_media_probe.h"
static uint32_t le32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
int spmp_stock_mp3_working_ram_recovered(uint32_t mem,uint32_t *a,uint32_t *b){
 if(!a||!b)return -1;
 /* Literals verified against .text at 0x0000 and pool at 0x07a8 of LIBMP3.O. */
 if(mem>=18432){*a=300;*b=18176;}
 else if(mem>=8192){*a=11564;*b=6912;}
 else if(mem>=4608){*a=13868;*b=4608;}
 else {*a=18476;*b=0;}
 return 0;
}
uint32_t spmp_stock_flac_working_ram_recovered(void){return 22728u;}
/* ISO 11172-3 and 13818-3, no floating-point arithmetic. */
int spmp_mp3_header(const uint8_t *p,size_t n,spmp_mp3_frame *out){
 static const uint16_t k1l1[16]={0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0};
 static const uint16_t k1l2[16]={0,32,48,56,64,80,96,112,128,160,192,224,256,320,384,0};
 static const uint16_t k1l3[16]={0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0};
 static const uint16_t k2l1[16]={0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0};
 static const uint16_t k2l2[16]={0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0};
 static const uint16_t k2l3[16]={0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0};
 static const uint32_t sr[3]={44100,48000,32000};
 uint32_t version,layer,idx,rateidx,bitrate,rate,samples,bytes,padding;
 const uint16_t *table;
 if(!p||!out||n<4)return -1;
 if(p[0]!=255||(p[1]&0xe0)!=0xe0)return -1;
 version=(p[1]>>3)&3;layer=(p[1]>>1)&3;
 idx=p[2]>>4;rateidx=(p[2]>>2)&3;padding=(p[2]>>1)&1;
 if(version==1||!layer||!idx||idx==15||rateidx==3)return -1;
 if(version==3)table=layer==3?k1l1:(layer==2?k1l2:k1l3);
 else table=layer==3?k2l1:(layer==2?k2l2:k2l3);
 bitrate=(uint32_t)table[idx]*1000;rate=sr[rateidx]>>(version==3?0:(version==2?1:2));
 if(!bitrate||!rate)return -1;
 samples=layer==3?384:(layer==2?1152:(version==3?1152:576));
 if(layer==3)bytes=(uint32_t)(((uint64_t)12*bitrate/rate)+padding)*4;
 else bytes=(uint32_t)(((uint64_t)(layer==1&&version!=3?72:144)*bitrate/rate)+padding);
 if(bytes<4||bytes>4096)return -1;
 out->sample_rate=rate;out->bit_rate_bps=bitrate;out->frame_bytes=bytes;
 out->samples_per_frame=samples;out->channels=(p[3]>>6)==3?1:2;
 out->layer=(uint8_t)(4-layer);
 out->mpeg_version=(uint8_t)(version==3?1:(version==2?2:25));
 return 0;
}
int spmp_mp3_find_frame(const uint8_t *p,size_t n,size_t *offset,spmp_mp3_frame *out){
 size_t i;if(!p||!offset||!out)return -1;
 for(i=0;i+4<=n;i++){
  spmp_mp3_frame x;
  if(spmp_mp3_header(p+i,n-i,&x))continue;
  /* Two-frame confirmation if enough buffered data. */
  if(x.frame_bytes+4<=n-i){spmp_mp3_frame y;
   if(spmp_mp3_header(p+i+x.frame_bytes,n-i-x.frame_bytes,&y)||
     x.sample_rate!=y.sample_rate||x.layer!=y.layer||x.mpeg_version!=y.mpeg_version)continue;
  }
  *offset=i;*out=x;return 0;
 }
 return -1;
}
int spmp_aac_adts_header(const uint8_t *p,size_t n,spmp_aac_adts *out){
 static const uint32_t rates[13]={96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350};
 unsigned id,profile,rate,ch,bytes,frames,header;
 if(!p||!out||n<7)return -1;
 if(p[0]!=0xff || (p[1]&0xf6)!=0xf0)return -1;
 id=(p[1]>>3)&1; (void)id;
 profile=(p[2]>>6)&3;rate=(p[2]>>2)&15;
 ch=((p[2]&1)<<2)|((p[3]>>6)&3);
 bytes=((p[3]&3)<<11)|(p[4]<<3)|(p[5]>>5);
 frames=(p[6]&3)+1;header=(p[1]&1)?7:9;
 if(rate>=13||profile>=3||ch<1||ch>2||bytes<header||frames>4||n<header)return -1;
 out->sample_rate=rates[rate];out->channels=(uint8_t)ch;
 out->object_type=(uint8_t)(profile+1);
 out->frame_bytes=bytes;out->samples_per_frame=1024*frames;
 out->header_bytes=(uint8_t)header;return 0;
}
int spmp_mmpi_inspect(const uint8_t *p,size_t n,spmp_mmpi *o){
 uint32_t total,record;
 if(!p||!o||n<52||n>0xffffffffu)return -1;
 if(p[0]!='M'||p[1]!='M'||p[2]!='P'||p[3]!='i'||
    p[16]!='M'||p[17]!='M'||p[18]!='P'||p[19]!='d')return -1;
 total=le32(p+4);record=le32(p+20);
 if(total!=n||record!=36||le32(p+40)!=0x00100000u)return -1;
 o->total_size=total;o->header_tag=le32(p+8);o->header_value=le32(p+12);
 o->block_record_size=record;o->module_family=p[35];o->module_slot=p[36];
 return 0;
}
