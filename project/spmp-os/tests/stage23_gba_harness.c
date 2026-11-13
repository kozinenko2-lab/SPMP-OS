#include "spmp_gba_cart.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
int main(void){
 uint8_t h[192]={0};
 const uint8_t logo[]={0x24,0xff,0xae,0x51,0x69,0x9a,0xa2,0x21};
 memcpy(h+4,logo,sizeof logo);h[0xb2]=0x96;
 unsigned s=0x19;for(unsigned i=0xa0;i<=0xbc;i++)s+=h[i];h[0xbd]=(uint8_t)(0u-s);
 assert(spmp_gba_validate_header(h,sizeof h)==0);
 assert(spmp_gba_validate_header(h,10)==-1);
 h[4]^=0x11;assert(spmp_gba_validate_header(h,sizeof h)==-2);h[4]^=0x11;
 h[0xb2]=0;assert(spmp_gba_validate_header(h,sizeof h)==-3);h[0xb2]=0x96;
 h[0xa0]^=1;assert(spmp_gba_validate_header(h,sizeof h)==-4);
 puts("Stage23: GBA cartridge header checks passed");
}
