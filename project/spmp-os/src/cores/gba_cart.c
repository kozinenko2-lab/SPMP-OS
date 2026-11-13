#include "spmp_gba_cart.h"
int spmp_gba_validate_header(const uint8_t *h,size_t n){
 static const uint8_t logo[]={0x24,0xff,0xae,0x51,0x69,0x9a,0xa2,0x21};
 if(!h||n<0xc0)return -1;
 for(unsigned i=0;i<sizeof logo;i++)if(h[4+i]!=logo[i])return -2;
 if(h[0xb2]!=0x96)return -3;
 unsigned sum=0x19;
 for(unsigned i=0xa0;i<=0xbc;i++)sum+=(unsigned)h[i];
 return (((sum+(unsigned)h[0xbd])&255u)==0)?0:-4;
}
