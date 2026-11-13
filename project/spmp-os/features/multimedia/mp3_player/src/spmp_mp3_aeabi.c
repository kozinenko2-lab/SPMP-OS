/* ARMv5 soft integer division and tiny libc, no allocator, GPL-2.0-or-later.
 * Use ONLY in freestanding SPMP-OS module link; not in hosted programs. */
#include <stdint.h>
#include <stddef.h>
#ifdef __arm__
void *memset(void *dst,int c,size_t n){uint8_t *p=dst;while(n--)*p++=(uint8_t)c;return dst;}
void *memcpy(void *dst,const void *src,size_t n){uint8_t *d=dst;const uint8_t *s=src;while(n--)*d++=*s++;return dst;}
void *memmove(void *dst,const void *src,size_t n){uint8_t *d=dst;const uint8_t *s=src;if(d<s){while(n--)*d++=*s++;}else if(d>s){while(n--){d[n]=s[n];}}return dst;}
int memcmp(const void *a,const void *b,size_t n){const uint8_t *x=a,*y=b;while(n--){if(*x!=*y)return (int)*x-(int)*y;x++;y++;}return 0;}
static uint32_t spmp_udiv32(uint32_t n,uint32_t d){
 uint32_t q=0,rem=0;unsigned i;
 if(d==0)return 0xffffffffu;
 for(i=0;i<32;i++){
  uint32_t high=rem>>31;rem=(rem<<1)|(n>>31);n<<=1;q<<=1;
  if(high||rem>=d){rem-=d;q|=1;}
 }
 return q;
}
uint32_t __aeabi_uidiv(uint32_t n,uint32_t d){return spmp_udiv32(n,d);}
int32_t __aeabi_idiv(int32_t n,int32_t d){
 uint32_t a=(uint32_t)n,b=(uint32_t)d,q;
 if(n<0)a=0u-a;if(d<0)b=0u-b;
 q=spmp_udiv32(a,b);
 return ((n<0)!=(d<0))?(int32_t)(0u-q):(int32_t)q;
}
/* Called from __aeabi_uldivmod veneer. Fifth arg is a 4-word output array. */
__attribute__((used)) void spmp_udivmod64(uint32_t nl,uint32_t nh,uint32_t dl,uint32_t dh,uint32_t out[4]){
 uint64_t n=((uint64_t)nh<<32)|nl,d=((uint64_t)dh<<32)|dl,q=0,r=0;
 unsigned i;
 if(!d){out[0]=out[1]=0xffffffffu;out[2]=nl;out[3]=nh;return;}
 for(i=0;i<64;i++){
  uint64_t carry=r>>63;
  r=(r<<1)|(n>>63);n<<=1;q<<=1;
  if(carry||r>=d){r-=d;q|=1;}
 }
 out[0]=(uint32_t)q;out[1]=(uint32_t)(q>>32);
 out[2]=(uint32_t)r;out[3]=(uint32_t)(r>>32);
}
#endif
