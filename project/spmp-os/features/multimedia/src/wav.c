#include "common.h"
int mm_play_wav(const mm_reader *r,const mm_audio_output *o,uint32_t *frames_done){
 uint64_t p;uint64_t data_at=0;uint32_t data_len=0,rate=0;unsigned channels=0,bits=0;
 uint8_t h[16];int have_fmt=0;unsigned loops=0;
 int16_t stereo[256*2];uint8_t raw[256*4];
 if(frames_done)*frames_done=0;
 if(!r||!o||!o->submit_s16||r->size<44||mm_read(r,0,h,12)||
    !same4(h,"RIFF")||!same4(h+8,"WAVE"))return -1;
 uint64_t limit=(uint64_t)rd32(h+4)+8u;
 if(limit<12||limit>r->size)return -1;
 p=12;
 while(p+8<=limit&&loops++<2048){
  if(mm_read(r,p,h,8))return -1;
  uint32_t len=rd32(h+4);uint64_t at=p+8;
  if(len>limit-at)return -1;
  if(same4(h,"fmt ")){
   if(len<16||mm_read(r,at,h,16)||rd16(h)!=1)return -1;
   channels=rd16(h+2);rate=rd32(h+4);bits=rd16(h+14);
   if((channels!=1&&channels!=2)||(bits!=8&&bits!=16)||rate<8000||rate>48000)return -1;
   have_fmt=1;
  }else if(same4(h,"data")&&!data_len){data_at=at;data_len=len;}
  p=at+len+(len&1u);
 }
 if(!have_fmt||!data_len)return -1;
 unsigned frame_bytes=channels*(bits/8u);
 if(data_len%frame_bytes)return -1;
 uint32_t done=0,frames=data_len/frame_bytes;
 while(done<frames){
  unsigned batch=frames-done>256?256:frames-done;
  size_t got=(size_t)batch*frame_bytes;
  if(mm_read(r,data_at+(uint64_t)done*frame_bytes,raw,got))return -1;
  for(unsigned i=0;i<batch;i++){
   unsigned index=i*frame_bytes;
   int16_t left,right;
   if(bits==8){
    left=(int16_t)(((int)raw[index]-128)*256);
    right=channels==1?left:(int16_t)(((int)raw[index+1]-128)*256);
   }else{
    left=(int16_t)rd16(raw+index);
    right=channels==1?left:(int16_t)rd16(raw+index+2);
   }
   stereo[i*2]=left;stereo[i*2+1]=right;
  }
  if(o->submit_s16(o->ctx,stereo,batch,rate)<0)return -2;
  done+=batch;if(frames_done)*frames_done=done;
 }
 return 0;
}
