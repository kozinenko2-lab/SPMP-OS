#include "spmp_sd_directory.h"
#include <stdint.h>
struct files_fingerprint {uint32_t address,bytes,crc32;};
#ifdef SPMP_BINDING_SYNTHETIC
#include "files_profile_synthetic.h"
#else
#include "files_profile.h"
#endif
uint32_t spmp_files_preflight(void){
 for(unsigned i=0;i<5;i++){
  const volatile uint8_t *p=(const volatile uint8_t *)(uintptr_t)files_prefixes[i].address;uint32_t crc=0xffffffffu;
  for(unsigned n=0;n<files_prefixes[i].bytes;n++){crc^=p[n];for(unsigned b=0;b<8;b++)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));}
  if((~crc)!=files_prefixes[i].crc32)return 0x90a;
 }
 return 0;
}
static int error(void){typedef const uint32_t *(*fn)(void);const uint32_t *p=((fn)(uintptr_t)0x299a14)();uint32_t n=p?*p:0;return n&&n<4096?-(int)n:-5;}
static int open_dir(void *user,const char *path){
 (void)user;typedef int (*fn)(const char *);int fd=((fn)(uintptr_t)0x2a69bc)(path);return fd>0?fd:error();
}
static int next(void *user,int fd,char name[260]){
 (void)user;typedef int (*fn)(int,void *,void **);void *result=0;
 for(unsigned i=0;i<260;i++)name[i]=0;
 int status=((fn)(uintptr_t)0x2a6b8c)(fd,name,&result);
 if(status)return status>0?-status:-5;
 if(!result)return 0;
 return result==name?1:-5;
}
static int stat_entry(void *user,const char *path,struct spmp_sd_meta *out){
 (void)user;typedef int (*fn)(const char *,void *);uint32_t st[8]={0};
 if(((fn)(uintptr_t)0x2a6504)(path,st))return error();
 out->mode=st[0];out->bytes=st[4];return 0;
}
static int close_dir(void *user,int fd){
 (void)user;typedef int (*fn)(int);return ((fn)(uintptr_t)0x2a6be0)(fd)?error():0;
}
void spmp_files_bind(struct spmp_sd_io *io){io->user=0;io->open=open_dir;io->next=next;io->stat=stat_entry;io->close=close_dir;}
