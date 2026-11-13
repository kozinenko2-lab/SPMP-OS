#include "spmp_md_rom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(void){
 uint8_t *raw=malloc(32768u),*smd=malloc(33280u),*work=malloc(16384u);
 assert(raw&&smd&&work);
 for(size_t i=0;i<32768u;i++)raw[i]=(uint8_t)((i*13u+i/17u)&255u);
 memcpy(raw+0x100,"SEGA",4);
 memcpy(smd+0,"\x01\x02",2);memset(smd+2,0,510);
 for(size_t base=0;base<32768u;base+=16384u){
  for(size_t i=0;i<8192u;i++){
   smd[512+base+i]=raw[base+2*i+1];
   smd[512+base+8192+i]=raw[base+2*i];
  }
 }
 assert(spmp_md_format(raw,32768u)==1);
 assert(spmp_md_format(smd,33280u)==2);
 assert(spmp_md_smd_prefix(smd,512u+16384u)==1);
 assert(spmp_md_smd_prefix(smd,512u+16383u)==0);
 assert(spmp_md_smd_prefix(raw,32768u)==0);
 size_t length=33280;
 assert(spmp_md_canonicalize(smd,&length,work,16384u)==0);
 assert(length==32768u && memcmp(raw,smd,length)==0);
 uint8_t invalid[1024]={0};size_t n=sizeof invalid;
 assert(spmp_md_format(invalid,n)==0);
 assert(spmp_md_canonicalize(invalid,&n,work,16384u)<0 && n==1024);
 puts("Mega Drive: raw header, 2-block SMD in-place conversion, malformed rejection OK");
 free(raw);free(smd);free(work);return 0;
}
