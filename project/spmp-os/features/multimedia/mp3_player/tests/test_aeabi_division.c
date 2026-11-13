/* Host checks mathematical helper; ARM asm veneer is link-checked separately. */
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
void spmp_udivmod64(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t[4]);
static uint64_t s=0xb5bcb0f7eb3b0241ULL;
static uint64_t rng(void){s^=s<<13;s^=s>>7;s^=s<<17;return s;}
int main(void){unsigned i;uint32_t v[4];
 for(i=0;i<30000;i++){
  uint64_t n=rng(),d=rng()|1,q,r;
  spmp_udivmod64((uint32_t)n,(uint32_t)(n>>32),(uint32_t)d,(uint32_t)(d>>32),v);
  q=((uint64_t)v[1]<<32)|v[0];r=((uint64_t)v[3]<<32)|v[2];
  assert(q==n/d && r==n%d);
 }
 puts("ARMv5 division algorithm PASS (30,000 cases)");return 0;
}
