#include "spmp_flac_lite.h"
#include <limits.h>

/* FLAC uses CRC-8 x^8+x^2+x+1 and CRC-16 x^16+x^15+x^2+1. */
uint8_t spmp_flac_crc8(const uint8_t *p, size_t n) {
 uint8_t c=0; size_t i; int j;
 if (!p && n) return 0;
 for(i=0;i<n;i++){c^=p[i];for(j=0;j<8;j++)c=(uint8_t)((c<<1)^((c&0x80)?7:0));}
 return c;
}
uint16_t spmp_flac_crc16(const uint8_t *p, size_t n) {
 uint16_t c=0; size_t i; int j;
 if (!p && n) return 0;
 for(i=0;i<n;i++){c^=(uint16_t)p[i]<<8;for(j=0;j<8;j++)c=(uint16_t)((c<<1)^((c&0x8000)?0x8005:0));}
 return c;
}
static uint32_t be24(const uint8_t *p){return ((uint32_t)p[0]<<16)|((uint32_t)p[1]<<8)|p[2];}
int spmp_flac_open(spmp_flac *d, const uint8_t *p, size_t n) {
 size_t off=4; int seen=0,last=0; unsigned block=0;
 if(!d || !p || n<8 || p[0]!='f'||p[1]!='L'||p[2]!='a'||p[3]!='C')return SPFL_BAD;
 d->data=p;d->size=n;d->first_frame=d->cursor=0;d->sample_rate=0;d->max_block=0;
 d->channels=0;d->bits_per_sample=0;d->total_samples=0;
 while(!last){
  uint8_t type;uint32_t length;
  if(off>n||n-off<4||++block>128)return SPFL_BAD;
  last=(p[off]&0x80)!=0;type=p[off++]&127;length=be24(p+off);off+=3;
  if(length>n-off)return SPFL_BAD;
  if(type==0){
   uint64_t v;
   if(seen || length!=34)return SPFL_BAD;
   seen=1;d->max_block=((uint32_t)p[off+2]<<8)|p[off+3];
   v=((uint64_t)p[off+10]<<56)|((uint64_t)p[off+11]<<48)|((uint64_t)p[off+12]<<40)|((uint64_t)p[off+13]<<32)|
     ((uint64_t)p[off+14]<<24)|((uint64_t)p[off+15]<<16)|((uint64_t)p[off+16]<<8)|p[off+17];
   d->sample_rate=(uint32_t)((v>>44)&0xfffff);
   d->channels=(uint8_t)(((v>>41)&7)+1);
   d->bits_per_sample=(uint8_t)(((v>>36)&31)+1);
   d->total_samples=v&0xfffffffffULL;
   if(!d->sample_rate||!d->max_block||d->max_block>65535||d->channels<1||d->channels>2||
       (d->bits_per_sample!=8&&d->bits_per_sample!=12&&d->bits_per_sample!=16&&d->bits_per_sample!=20&&d->bits_per_sample!=24))return SPFL_UNSUPPORTED;
  }
  off+=(size_t)length;
 }
 if(!seen || off>n)return SPFL_BAD;
 d->first_frame=d->cursor=off;return SPFL_OK;
}
void spmp_flac_rewind(spmp_flac *d){if(d)d->cursor=d->first_frame;}
size_t spmp_flac_work_size(uint32_t max_block){if(max_block>65535)return 0;return (size_t)max_block*2u*sizeof(int32_t);}

typedef struct bits {const uint8_t *p;size_t bytes,pos;int error;} bits;
static uint32_t rd(bits *b,unsigned n){
 uint32_t x=0;unsigned i;size_t idx;unsigned shift;
 if(n>32||b->error||b->pos>b->bytes*8 || n>b->bytes*8-b->pos){b->error=1;return 0;}
 if(n==0)return 0;
 if(n==1){x=(b->p[b->pos>>3]>>(7-(b->pos&7)))&1;b->pos++;return x;}
 idx=b->pos>>3;shift=(unsigned)(b->pos&7);
 if(n<=32-shift && idx+4<=b->bytes){
  uint32_t word=((uint32_t)b->p[idx]<<24)|((uint32_t)b->p[idx+1]<<16)|
                 ((uint32_t)b->p[idx+2]<<8)|b->p[idx+3];
  b->pos+=n;return (word<<shift)>>(32-n);
 }
 for(i=0;i<n;i++){x=(x<<1)|((b->p[b->pos>>3]>>(7-(b->pos&7)))&1);b->pos++;}
 return x;
}
static int32_t signex(uint32_t x,unsigned bits){
 if(!bits)return 0;
 if(bits==32)return (int32_t)x;
 if(x&(1u<<(bits-1)))x|=~((1u<<bits)-1);
 return (int32_t)x;
}
static int64_t sar64(int64_t x,unsigned s) {
 /* C right shift of negative is implementation-defined: avoid dependence */
 if(!s)return x;
 if(s>=63)return x<0?-1:0;
 return x>=0?x>>s:-(((-x-1)>>s)+1);
}
static int unary(bits *b,uint32_t max){uint32_t n=0;for(;;){
 if(b->error||n>max){b->error=1;return 0;}
 if(rd(b,1))return (int)n;
 n++;
}}
static int32_t rice(bits *b,unsigned k){
 uint32_t q,r,v;int u=unary(b,1<<20);if(b->error||k>30)return 0;
 q=(uint32_t)u;
 if(q>(0xffffffffu>>k)){b->error=1;return 0;}
 r=rd(b,k);v=(q<<k)|r;
 return (v&1)?-(int32_t)(v>>1)-1:(int32_t)(v>>1);
}
static int decode_sub(bits *b,int32_t *dst,unsigned block,int bps) {
 unsigned i,t,order=0,wasted=0;int32_t coeff[32];int lpc_shift=0;
 if(rd(b,1)!=0)return SPFL_BAD;
 t=rd(b,6);
 if(rd(b,1)) {wasted=1+(unsigned)unary(b,32);}
 if(b->error||wasted>=(unsigned)bps || bps>32)return SPFL_BAD;
 bps-=(int)wasted;
 if(t==0){int32_t v=signex(rd(b,(unsigned)bps),(unsigned)bps);for(i=0;i<block;i++)dst[i]=v;}
 else if(t==1){for(i=0;i<block;i++)dst[i]=signex(rd(b,(unsigned)bps),(unsigned)bps);}
 else if((t>=8&&t<=12)||(t>=32&&t<=63)){
  unsigned method,param_bits,po,parts,partition;
  int is_lpc=t>=32;
  order=is_lpc?(t&31)+1:t-8;
  if(order>block||order>32)return SPFL_BAD;
  for(i=0;i<order;i++)dst[i]=signex(rd(b,(unsigned)bps),(unsigned)bps);
  if(is_lpc){
   unsigned prec=rd(b,4)+1;
   if(prec==16)return SPFL_BAD;
   lpc_shift=signex(rd(b,5),5);
   for(i=0;i<order;i++)coeff[i]=signex(rd(b,prec),prec);
  }
  method=rd(b,2);po=rd(b,4);
  if(method>1 || po>12)return SPFL_UNSUPPORTED;
  param_bits=method==0?4:5;parts=1u<<po;
  if(block%parts || block/parts<order)return SPFL_BAD;
  for(partition=0;partition<parts;partition++){
   unsigned n=block/parts-(partition==0?order:0),k=rd(b,param_bits),j;
   unsigned first=(partition*block/parts)+(partition==0?order:0);
   unsigned escape=(1u<<param_bits)-1;
   for(j=0;j<n;j++){
    int32_t residual;
    unsigned at=first+j;
    int64_t prediction=0,z;
    if(k==escape){unsigned rawbits=rd(b,5);residual=signex(rd(b,rawbits),rawbits);}
    else residual=rice(b,k);
    if(b->error)return SPFL_BAD;
    if(is_lpc){unsigned h;
     for(h=0;h<order;h++)prediction+=(int64_t)coeff[h]*dst[at-h-1];
     if(lpc_shift>=0)prediction=sar64(prediction,(unsigned)lpc_shift);
     else {unsigned sh=(unsigned)(-lpc_shift); if(sh>15 || prediction>(INT32_MAX>>sh) || prediction<(INT32_MIN>>sh))return SPFL_UNSUPPORTED;prediction*=((int64_t)1<<sh);}
    }else{
     int64_t a=at>=1?dst[at-1]:0,c=at>=2?dst[at-2]:0,d=at>=3?dst[at-3]:0,e=at>=4?dst[at-4]:0;
     switch(order){case 0:prediction=0;break;case 1:prediction=a;break;case 2:prediction=2*a-c;break;case 3:prediction=3*a-3*c+d;break;case 4:prediction=4*a-6*c+4*d-e;break;default:return SPFL_BAD;}
    }
    z=prediction+residual;
    if(z>INT32_MAX || z<INT32_MIN)return SPFL_UNSUPPORTED;
    dst[at]=(int32_t)z;
   }
  }
 }else return SPFL_UNSUPPORTED;
 if(b->error)return SPFL_BAD;
 if(wasted){for(i=0;i<block;i++){
  int64_t v=(int64_t)dst[i]*(1LL<<wasted);
  if(v>INT32_MAX||v<INT32_MIN)return SPFL_UNSUPPORTED;
  dst[i]=(int32_t)v;
 }}
 return SPFL_OK;
}
static int utf8_counter(bits *b){
 uint8_t a=(uint8_t)rd(b,8);unsigned count,i;
 if(a<0x80)return b->error?SPFL_BAD:SPFL_OK;
 if((a&0xe0)==0xc0)count=1;
 else if((a&0xf0)==0xe0)count=2;
 else if((a&0xf8)==0xf0)count=3;
 else if((a&0xfc)==0xf8)count=4;
 else if((a&0xfe)==0xfc)count=5;
 else if(a==0xfe)count=6;
 else return SPFL_BAD;
 for(i=0;i<count;i++)if((rd(b,8)&0xc0)!=0x80)return SPFL_BAD;
 return b->error?SPFL_BAD:SPFL_OK;
}
static int16_t s16_sample(int64_t sample,unsigned bps){
 if(bps<16)sample*=((int64_t)1<<(16-bps));
 else if(bps>16)sample=sar64(sample,bps-16);
 if(sample>32767)sample=32767;
 if(sample<-32768)sample=-32768;
 return (int16_t)sample;
}
int spmp_flac_decode_frame(spmp_flac *d,void *scratch,size_t scratch_bytes,int16_t *dst,size_t capacity,size_t *out_frames){
 bits b;size_t start,header_end,crc_end;unsigned fblock,br,rate,ch_assign,samplebits,ch,i;int32_t *left,*right;int ret;
 if(out_frames)*out_frames=0;
 if(!d||!scratch||!dst||!out_frames||!d->data)return SPFL_BAD;
 if(d->cursor>=d->size)return SPFL_EOF;
 start=d->cursor;b.p=d->data+start;b.bytes=d->size-start;b.pos=0;b.error=0;
 if(rd(&b,14)!=0x3ffe || rd(&b,1)!=0)return SPFL_BAD;
 (void)rd(&b,1); /* frame-number vs sample-number */
 br=rd(&b,4);rate=rd(&b,4);ch_assign=rd(&b,4);samplebits=rd(&b,3);
 if(rd(&b,1)!=0 || utf8_counter(&b)!=SPFL_OK)return SPFL_BAD;
 if(br==0)return SPFL_BAD;
 if(br==1)fblock=192;
 else if(br>=2 && br<=5)fblock=576u<<(br-2);
 else if(br==6)fblock=rd(&b,8)+1;
 else if(br==7)fblock=rd(&b,16)+1;
 else fblock=256u<<(br-8);
 if(rate==12)(void)rd(&b,8);else if(rate==13||rate==14)(void)rd(&b,16);
 if(rate==15||ch_assign>10||samplebits==3||samplebits==7)return SPFL_UNSUPPORTED;
 if(ch_assign<8 && ch_assign+1!=d->channels)return SPFL_UNSUPPORTED;
 if(ch_assign>=8 && d->channels!=2)return SPFL_UNSUPPORTED;
 if((b.pos&7)||b.error)return SPFL_BAD;
 header_end=b.pos/8;
 if(header_end>=b.bytes || spmp_flac_crc8(b.p,header_end)!=b.p[header_end])return SPFL_CRC;
 (void)rd(&b,8);
 if(fblock>d->max_block || scratch_bytes<spmp_flac_work_size(fblock) ||
    ((uintptr_t)scratch&3u) || capacity<(size_t)fblock*d->channels)return SPFL_SMALL;
 left=(int32_t*)scratch;right=left+fblock;
 for(ch=0;ch<d->channels;ch++){
  int bps=d->bits_per_sample + (ch_assign==8&&ch==1) + (ch_assign==9&&ch==0) + (ch_assign==10&&ch==1);
  ret=decode_sub(&b,ch?right:left,fblock,bps);
  if(ret!=SPFL_OK)return ret;
 }
 if(b.error)return SPFL_BAD;
 while(b.pos&7)(void)rd(&b,1);
 crc_end=b.pos/8;
 if(crc_end+2>b.bytes)return SPFL_BAD;
 if(spmp_flac_crc16(b.p,crc_end)!=((uint16_t)b.p[crc_end]<<8|b.p[crc_end+1]))return SPFL_CRC;
 b.pos+=16;
 for(i=0;i<fblock;i++){
  int64_t x=left[i],y=right[i];
  if(ch_assign==8)y=x-y;
  else if(ch_assign==9)x=x+y;
  else if(ch_assign==10){x=2*x+(y&1); x=(x+y)/2; y=x-y;}
  dst[i*d->channels]=s16_sample(x,d->bits_per_sample);
  if(d->channels==2)dst[i*2+1]=s16_sample(y,d->bits_per_sample);
 }
 d->cursor=start+b.pos/8;*out_frames=fblock;return SPFL_OK;
}

int spmp_flac_decode_chunk(const spmp_flac *meta,
                           const uint8_t *compressed, size_t compressed_bytes,
                           void *scratch, size_t scratch_bytes,
                           int16_t *interleaved, size_t capacity,
                           size_t *consumed, size_t *out_frames) {
 spmp_flac local;int rc;
 if(!meta||!compressed||!consumed||!out_frames)return SPFL_BAD;
 *consumed=0;*out_frames=0;
 local=*meta;local.data=compressed;local.size=compressed_bytes;
 local.first_frame=0;local.cursor=0;
 rc=spmp_flac_decode_frame(&local,scratch,scratch_bytes,interleaved,capacity,out_frames);
 if(rc==SPFL_OK)*consumed=local.cursor;
 return rc;
}
