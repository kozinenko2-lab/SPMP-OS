/* ARM926EJ-S has no hardware integer divide. Standalone EABI fallbacks.
   This file is linked only for ARM bare-metal relocatable objects. */
#include <stddef.h>
#include <stdint.h>
__attribute__((used)) unsigned __aeabi_uidiv(unsigned a,unsigned b){
 if(!b)return 0; /* Caller must validate divisors. */
 unsigned result=0, remainder=0;
 for(int bit=31;bit>=0;bit--){
  unsigned carry=remainder>>31;
  remainder=(remainder<<1)|((a>>(unsigned)bit)&1u);
  if(carry||remainder>=b){remainder-=b;result|=1u<<(unsigned)bit;}
 }
 return result;
}
__attribute__((used)) void __aeabi_memcpy(void *dst,const void *src,size_t n){
 volatile uint8_t *d=(volatile uint8_t*)dst;
 const volatile uint8_t *s=(const volatile uint8_t*)src;
 for(size_t i=0;i<n;i++)d[i]=s[i];
}
