#include "spmp_theme_config_v9.h"
#include "spmp_theme_catalog_v9.h"
#include <assert.h>
int main(void){unsigned idx=999;uint8_t layout=9;uint8_t mem[40];
 for(unsigned n=0;n<spmp_theme_v9_count();n++)for(unsigned l=0;l<=1;l++){
  assert(!spmp_theme_v9_config_encode(spmp_theme_v9_id(n),(uint8_t)l,mem));
  assert(!spmp_theme_v9_config_decode(mem,&idx,&layout));assert(idx==n&&layout==l);
  mem[14]^=1;assert(spmp_theme_v9_config_decode(mem,&idx,&layout)<0);mem[14]^=1;
 }
 assert(spmp_theme_v9_config_encode("../../foo",0,mem)<0);
 assert(spmp_theme_v9_cycle(0,-1)==33);assert(spmp_theme_v9_cycle(33,1)==0);
 assert(spmp_theme_v9_cycle(2,-2147483647-1)==(unsigned)(2+34-(2147483648u%34u))%34u);
 return 0;}
