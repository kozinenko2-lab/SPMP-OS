/* SPMP-OS Mega Drive cartridge preprocessor (not a CPU emulator). */
#include "spmp_md_rom.h"
#include <string.h>
#define MD_BLOCK 16384u
#define MD_HALF 8192u
#define MD_HEADER 512u
static int header(const uint8_t *b,size_t n){
 return n>=0x104 && memcmp(b+0x100,"SEGA",4)==0;
}
int spmp_md_smd_prefix(const uint8_t *b,size_t n){
 if(!b||n<MD_HEADER+MD_BLOCK)return 0;
 const uint8_t *blk=b+MD_HEADER;
 return blk[MD_HALF+0x80]=='S'&&blk[0x80]=='E'&&
        blk[MD_HALF+0x81]=='G'&&blk[0x81]=='A';
}
int spmp_md_format(const uint8_t *b,size_t n){
 if(!b)return 0;
 if(header(b,n))return 1;
 if(n<MD_HEADER+MD_BLOCK || (n-MD_HEADER)%MD_BLOCK!=0)return 0;
 /* Decode the copier block which contains ROM's 0x100 SEGA header:
  * even bytes in second half, odd bytes in first. */
 return spmp_md_smd_prefix(b,n)?2:0;
}
int spmp_md_canonicalize(uint8_t *b,size_t *n,uint8_t *work,size_t cap){
 if(!b||!n)return -1;
 int type=spmp_md_format(b,*n);
 if(type==1)return 0;
 if(type!=2||!work||cap<MD_BLOCK)return -1;
 size_t length=*n-MD_HEADER;
 /* All checks above precede in-place writes. Work buffer must not alias ROM. */
 if((uintptr_t)work<(uintptr_t)b+*n && (uintptr_t)b<(uintptr_t)work+MD_BLOCK)return -1;
 for(size_t base=0;base<length;base+=MD_BLOCK){
  memcpy(work,b+MD_HEADER+base,MD_BLOCK);
  for(size_t i=0;i<MD_HALF;i++){
   b[base+i*2u]=work[MD_HALF+i];
   b[base+i*2u+1]=work[i];
  }
 }
 *n=length;
 return header(b,length)?0:-1;
}
