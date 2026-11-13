#include "common.h"

const char *mm_type_name(mm_type t){
 switch(t){case MM_MP3:return "MP3";case MM_WAV:return "WAV";
 case MM_MP4:return "MP4";case MM_AVI:return "AVI";case MM_BMP:return "BMP";
 case MM_JPEG:return "JPEG";case MM_PNG:return "PNG";case MM_GIF:return "GIF";
 default:return "unknown";}
}
static int good_image(mm_info *v,uint32_t w,uint32_t h){
 if(!w||!h||w>SPMP_MM_MAX_PHOTO_SIDE||h>SPMP_MM_MAX_PHOTO_SIDE||
    w*h>SPMP_MM_MAX_PHOTO_PIXELS)return -1;
 v->width=w;v->height=h;return 0;
}
static void copy_codec(char out[12],const uint8_t *src){mm_copy4(out,src);}
static int parse_wav(const mm_reader *r,mm_info *out){
 uint64_t p=12,limit=r->size;unsigned loops=0;int have_fmt=0;
 uint8_t riff[12];
 if(limit<12||mm_read(r,0,riff,12))return -1;
 uint64_t riff_bytes=(uint64_t)rd32(riff+4)+8u;
 if(riff_bytes>limit||riff_bytes<12)return -1;
 limit=riff_bytes;
 while(p<=limit && limit-p>=8 && loops++<2048){
  uint8_t chunk[24];if(mm_read(r,p,chunk,8))return -1;
  uint32_t len=rd32(chunk+4);uint64_t start=p+8, end=start+(uint64_t)len;
  if(end>limit)return -1;
  if(same4(chunk,"fmt ")){
   if(len<16||mm_read(r,start,chunk,16))return -1;
   if(rd16(chunk)!=1)return -1; /* PCM only */
   out->channels=rd16(chunk+2);out->sample_rate=rd32(chunk+4);
   out->bits_per_sample=rd16(chunk+14);
   if(out->channels<1||out->channels>2||out->sample_rate<8000||
      out->sample_rate>48000||(out->bits_per_sample!=8&&out->bits_per_sample!=16))return -1;
   copy_codec(out->audio_codec,(const uint8_t*)"PCM ");have_fmt=1;
  }else if(same4(chunk,"data")){
   if(len>out->data_bytes)out->data_bytes=len;
  }
  p=end+(len&1u);
 }
 if(!have_fmt||!out->data_bytes)return -1;
 uint32_t frame_bytes=out->channels*(out->bits_per_sample/8u);
 if(!frame_bytes||out->data_bytes%frame_bytes)return -1;
 uint32_t frames=out->data_bytes/frame_bytes;
 uint32_t sec=frames/out->sample_rate,rem=frames%out->sample_rate;
 uint64_t duration=(uint64_t)sec*1000u+(rem*1000u/out->sample_rate);
 if(duration<=UINT32_MAX)out->duration_ms=(uint32_t)duration;
 return 0;
}
/* Recursion is limited to the MP4 tree, and all atoms are bounds-checked. */
static int scan_boxes(const mm_reader *r,uint64_t start,uint64_t end,
                      unsigned depth,mm_info *out,unsigned *budget){
 uint64_t p=start;
 if(depth>6)return -1;
 while(p<end){
  uint8_t head[16];uint64_t len,header=8;
  if(end-p<8||!(*budget)--||mm_read(r,p,head,8))return -1;
  len=be32(head);
  if(len==1){if(end-p<16||mm_read(r,p+8,head+8,8))return -1;
   len=((uint64_t)be32(head+8)<<32)|be32(head+12);header=16;
  }else if(len==0)len=end-p;
  if(len<header||len>end-p)return -1;
  if(same4(head+4,"moov")||same4(head+4,"trak")||
     same4(head+4,"mdia")||same4(head+4,"minf")||
     same4(head+4,"stbl")){
   if(scan_boxes(r,p+header,p+len,depth+1,out,budget))return -1;
  }else if(same4(head+4,"stsd")&&len>=header+16){
   uint8_t entry[36];uint64_t entry_at=p+header+8;
   if(mm_read(r,entry_at,entry,8))return -1;
   uint32_t entry_size=be32(entry);
   if(entry_size>=8 && entry_size<=p+len-entry_at){
    const uint8_t *tag=entry+4;
    if(same4(tag,"avc1")||same4(tag,"avc3")||same4(tag,"mp4v")||
       same4(tag,"hvc1")||same4(tag,"hev1")||same4(tag,"av01")||
       same4(tag,"jpeg")||same4(tag,"mjpg")){
     copy_codec(out->video_codec,tag);
     if(entry_size>=36&&mm_read(r,entry_at,entry,36)==0){
      uint32_t w=be16(entry+32),h=be16(entry+34);
      if(w&&h&&good_image(out,w,h)){
        /* video may exceed a still-photo bound; keep codec without size */
        out->width=out->height=0;
      }
     }
    }else if(same4(tag,"mp4a")||same4(tag,".mp3")||
              same4(tag,"alac")||same4(tag,"Opus")||same4(tag,"ac-3")){
      copy_codec(out->audio_codec,tag);
    }
   }
  }
  p+=len;
 }
 return 0;
}
static int parse_mp4(const mm_reader *r,mm_info *out){
 unsigned budget=512;uint8_t first[12];
 if(r->size<16||mm_read(r,0,first,12))return -1;
 if(!same4(first+4,"ftyp")||be32(first)<16||be32(first)>r->size)return -1;
 mm_copy4(out->label,first+8);
 return scan_boxes(r,0,r->size,0,out,&budget);
}
static int parse_jpeg(const mm_reader *r,mm_info *out){
 uint64_t p=2;unsigned budget=8192;
 while(p+4<=r->size&&budget--){
  uint8_t b[4];if(mm_read(r,p,b,4))return -1;
  if(b[0]!=0xff)return -1;
  if(b[1]==0xff){p++;continue;}
  if(b[1]==0xd9||b[1]==0xda)break;
  uint32_t len=be16(b+2);
  if(len<2||len>r->size-p-2)return -1;
  if((b[1]>=0xc0&&b[1]<=0xc3)||(b[1]>=0xc5&&b[1]<=0xc7)||
     (b[1]>=0xc9&&b[1]<=0xcb)||(b[1]>=0xcd&&b[1]<=0xcf)){
   uint8_t sz[5];if(len<7||mm_read(r,p+4,sz,5))return -1;
   return good_image(out,be16(sz+3),be16(sz+1));
  }
  p+=2u+len;
 }
 return -1;
}
static int parse_mp3(const mm_reader *r,mm_info *out){
 uint8_t h[10];uint64_t at=0;
 if(mm_read(r,0,h,4))return -1;
 if(h[0]=='I'&&h[1]=='D'&&h[2]=='3'){
  if(r->size<10||mm_read(r,0,h,10))return -1;
  for(unsigned i=6;i<10;i++)if(h[i]&0x80u)return -1;
  uint32_t size=((uint32_t)h[6]<<21)|((uint32_t)h[7]<<14)|((uint32_t)h[8]<<7)|h[9];
  at=10u+(uint64_t)size+((h[5]&0x10u)?10u:0u);
 }
 if(at+4>r->size)return -1;
 /* Small scan covers optional junk/padding before first MPEG frame. */
 for(unsigned scanned=0;scanned<4096&&at+4<=r->size;scanned++,at++){
  if(mm_read(r,at,h,4))return -1;
  uint32_t v=be32(h);
  if((v&0xffe00000u)!=0xffe00000u)continue;
  unsigned ver=(v>>19)&3u, layer=(v>>17)&3u, bitrate=(v>>12)&15u, freq=(v>>10)&3u;
  if(ver==1||layer!=1||bitrate==0||bitrate==15||freq==3)continue;
  static const unsigned rates[3]={44100,48000,32000};
  out->sample_rate=rates[freq];
  if(ver==2)out->sample_rate/=2;
  else if(ver==0)out->sample_rate/=4;
  out->channels=((v>>6)&3u)==3?1u:2u;
  mm_copy4(out->audio_codec,(const uint8_t*)"MP3 ");
  return 0;
 }
 return -1;
}
int mm_inspect(const mm_reader *r,mm_info *out){
 uint8_t h[40];size_t n;
 if(!r||!r->read_at||!out||r->size<4)return -1;
 mm_zero(out,sizeof(*out));n=r->size<sizeof(h)?(size_t)r->size:sizeof(h);
 mm_zero(h,sizeof h);if(mm_read(r,0,h,n))return -1;
 if(n>=24&&same4(h,"RIFF")&&same4(h+8,"WAVE")){
  out->type=MM_WAV;return parse_wav(r,out);
 }
 if(n>=12&&same4(h,"RIFF")&&same4(h+8,"AVI ")){
  out->type=MM_AVI;return 0;
 }
 if(n>=12&&same4(h+4,"ftyp")){
  out->type=MM_MP4;return parse_mp4(r,out);
 }
 if(n>=24&&h[0]==0x89&&same4(h+1,"PNG\r")&&h[5]=='\n'&&h[6]==26&&h[7]=='\n'){
  if(!same4(h+12,"IHDR")||be32(h+8)!=13)return -1;
  out->type=MM_PNG;return good_image(out,be32(h+16),be32(h+20));
 }
 if(n>=10&&h[0]=='G'&&h[1]=='I'&&h[2]=='F'&&h[3]=='8'&&
    (h[4]=='7'||h[4]=='9')&&h[5]=='a'){
  out->type=MM_GIF;return good_image(out,rd16(h+6),rd16(h+8));
 }
 if(n>=26&&h[0]=='B'&&h[1]=='M'){
  uint32_t dib=rd32(h+14);
  if(dib<40||n<26)return -1;
  uint32_t w=rd32(h+18),hh=rd32(h+22);
  out->type=MM_BMP;
  return good_image(out,w,hh&0x80000000u?(~hh+1u):hh);
 }
 if(n>=4&&h[0]==0xff&&h[1]==0xd8){out->type=MM_JPEG;return parse_jpeg(r,out);}
 if((n>=3&&h[0]=='I'&&h[1]=='D'&&h[2]=='3')||
    (n>=2&&h[0]==0xff&&(h[1]&0xe0u)==0xe0u)){
  out->type=MM_MP3;return parse_mp3(r,out);
 }
 return -1;
}
