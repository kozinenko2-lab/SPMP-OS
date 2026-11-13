#include "spmp_ima_adpcm.h"
static const int idxdelta[16]={-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8};
static const int step[89]={7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767};
static int16_t i16(const uint8_t *p){return (int16_t)(p[0] | ((unsigned)p[1]<<8));}
static int clamp(int v,int lo,int hi){return v<lo?lo:v>hi?hi:v;}
static int16_t nibble(int n,int *pred,int *index){int diff=step[*index]>>3;
 if(n&1)diff+=step[*index]>>2;if(n&2)diff+=step[*index]>>1;
 if(n&4)diff+=step[*index];if(n&8)diff=-diff;
 *pred=clamp(*pred+diff,-32768,32767);
 *index=clamp(*index+idxdelta[n&15],0,88);
 return (int16_t)*pred;}
int sp_ima_decode_block(const uint8_t *src,size_t bytes,unsigned ch,int16_t *dst,size_t cap){
 int pr[2],idx[2];size_t i,frames=1,pos[2]={1,1};
 if(!src||!dst||(ch!=1&&ch!=2)||bytes<ch*4||bytes%4!=0)return -1;
 for(i=0;i<ch;i++){pr[i]=i16(src+i*4);idx[i]=src[i*4+2];if(idx[i]>88||src[i*4+3]!=0)return -1;
  if(i>=cap)return -1;dst[i]=(int16_t)pr[i];}
 if(ch==1){for(i=4;i<bytes;i++){unsigned b=src[i];if((frames+2)>cap)return -1;
  dst[frames++]=nibble(b&15,&pr[0],&idx[0]);dst[frames++]=nibble(b>>4,&pr[0],&idx[0]);}
 }else{for(i=8;i<bytes;i+=8){size_t j,k;
   if(bytes-i<8||(frames+8)*ch>cap)return -1;
   for(j=0;j<2;j++)for(k=0;k<4;k++){unsigned b=src[i+j*4+k];
    dst[(frames+2*k)*2+j]=nibble(b&15,&pr[j],&idx[j]);
    dst[(frames+2*k+1)*2+j]=nibble(b>>4,&pr[j],&idx[j]);}
   frames+=8;}}
 return (int)frames;
}
