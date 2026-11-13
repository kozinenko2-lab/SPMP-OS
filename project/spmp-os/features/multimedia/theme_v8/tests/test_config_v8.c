#include "spmp_theme_config_v8.h"
#include "spmp_theme_catalog_v8.h"
#include <assert.h>
int main(void){unsigned idx=999;uint8_t layout=9;uint8_t mem[40];
 for(unsigned n=0;n<spmp_theme_v8_count();n++)for(unsigned l=0;l<=1;l++){
  assert(!spmp_theme_v8_config_encode(spmp_theme_v8_id(n),(uint8_t)l,mem));
  assert(!spmp_theme_v8_config_decode(mem,&idx,&layout));assert(idx==n&&layout==l);
  mem[14]^=1;assert(spmp_theme_v8_config_decode(mem,&idx,&layout)<0);mem[14]^=1;
 }
 assert(spmp_theme_v8_config_encode("../../foo",0,mem)<0);
 assert(spmp_theme_v8_cycle(0,-1)==24);assert(spmp_theme_v8_cycle(24,1)==0);
 assert(spmp_theme_v8_cycle(2,-2147483647-1)==(unsigned)(2+25-(2147483648u%25u))%25u);
 return 0;}
