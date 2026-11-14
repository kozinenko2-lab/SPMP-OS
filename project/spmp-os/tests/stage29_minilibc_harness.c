#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern void __aeabi_memcpy(void *,const void *,size_t);
extern void __aeabi_memcpy4(void *,const void *,size_t);
extern void __aeabi_memcpy8(void *,const void *,size_t);
extern void __aeabi_memmove(void *,const void *,size_t);
extern void __aeabi_memmove4(void *,const void *,size_t);
extern void __aeabi_memset(void *,size_t,int);
extern void __aeabi_memset4(void *,size_t,int);
extern void __aeabi_memclr(void *,size_t);
extern void __aeabi_memclr4(void *,size_t);
extern void __aeabi_memclr8(void *,size_t);
static unsigned long checks;
static void insist(int x, const char *msg) { ++checks; if(!x){ fprintf(stderr,"FAIL %s\n",msg); exit(1); } }
int main(void) {
  unsigned char a[128],b[128],ref[128];
  for(int i=0;i<128;i++) a[i]=b[i]=ref[i]=(unsigned char)(i*39);
  for(int off=0;off<36;off++) for(int n=0;n<64;n++) {
    memcpy(b,a,128); memcpy(ref,a,128);
    /* independent reference using a scratch buffer */
    unsigned char tmp[128]; for(int i=0;i<n;i++)tmp[i]=ref[i];
    for(int i=0;i<n;i++)ref[off+i]=tmp[i];
    __aeabi_memmove(b+off,b,n);
    insist(memcmp(b,ref,128)==0,"memmove overlap");
    memcpy(b,a,128); memcpy(ref,a,128);
    for(int i=0;i<n;i++)tmp[i]=ref[off+i];
    for(int i=0;i<n;i++)ref[i]=tmp[i];
    __aeabi_memmove4(b,b+off,n);
    insist(memcmp(b,ref,128)==0,"memmove backward");
  }
  for(int n=0;n<128;n++) {
    memset(b,0x53,128);
    __aeabi_memclr4(b,n);
    for(int i=0;i<128;i++)insist(b[i]==(i<n?0:0x53),"memclr4");
    __aeabi_memset(b,n,0xa7);
    for(int i=0;i<128;i++)insist(b[i]==(i<n?0xa7:0x53),"memset order");
    __aeabi_memset4(b,n,0x19);
    for(int i=0;i<128;i++)insist(b[i]==(i<n?0x19:0x53),"memset4");
  }
  __aeabi_memcpy(b,a,128);insist(memcmp(b,a,128)==0,"copy");
  __aeabi_memcpy4(b,a,128);insist(memcmp(b,a,128)==0,"copy4");
  __aeabi_memcpy8(b,a,128);insist(memcmp(b,a,128)==0,"copy8");
  __aeabi_memclr(b,128);for(int i=0;i<128;i++)insist(b[i]==0,"clr");
  memset(b,0x53,128);__aeabi_memclr8(b,120);
  for(int i=0;i<128;i++)insist(b[i]==(i<120?0:0x53),"clr8 boundary");
  char s[64];strcpy(s,"abc");strcat(s,"XYZ");
  insist(strcmp(s,"abcXYZ")==0,"strings");
  insist(strlen(s)==6,"strlen");
  insist(strrchr(s,'X')==s+3,"strrchr");
  insist(strrchr(s,'Q')==NULL,"strrchr miss");
  insist(strrchr(s,0)==s+6,"strrchr null");
  insist(strncmp(s,"abc0",3)==0,"strncmp");
  insist(memchr(s,'X',6)==s+3,"memchr");
  puts("PASS minilibc + EABI memory helpers"); printf("checks=%lu\n",checks);
  return 0;
}
