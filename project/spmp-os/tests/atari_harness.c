/* Deterministic Atari 2600 test, no copyrighted ROMs. */
#include "spmp_cores.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
extern unsigned char RiotRam[128];
extern unsigned int reg_pc;
extern int Frame;
static unsigned rendered,errors,reads;
static const char *private_rom;
static const char first[]="User/ROMs/Atari2600/First game.a26";
static const char second[]="Different folder/Nested/Second renamed.a26";
static void screen(void *user,const uint16_t *pixels,unsigned w,unsigned h){
 (void)user;
 if(!pixels||w!=320||h!=240)errors++;
 rendered++;
}
static uint32_t keys(void *user){(void)user;return 0;}
static int get_rom(void *user,const char *path,uint8_t *dst,size_t cap,size_t *got){
 (void)user;
 if(private_rom){
  FILE *file=fopen(path,"rb");if(!file)return -1;
  size_t n=fread(dst,1,cap,file);int extra=fgetc(file);int close=fclose(file);
  if(extra!=EOF||close)return -1;
  *got=n;reads++;return 0;
 }
 if((strcmp(path,first)&&strcmp(path,second)) || cap<4096)return -1;
 reads++;
 memset(dst,0xea,4096);
 const uint8_t code[]={0xa9,0x42,0x85,0x80,0x4c,0x04,0xf0};
 memcpy(dst,code,sizeof code);
 if(!strcmp(path,second))dst[1]=0x24;
 dst[0xffc]=0x00;dst[0xffd]=0xf0;dst[0xffe]=0x00;dst[0xfff]=0xf0;
 *got=4096;return 0;
}
static int file_size(void *user,const char *path,size_t *bytes){
 (void)user;
 if(private_rom){FILE *file=fopen(path,"rb");if(!file)return -1;int rc=fseek(file,0,SEEK_END);long n=ftell(file);int close=fclose(file);if(rc||n<0||close)return -1;*bytes=(size_t)n;return 0;}
 if(!strcmp(path,first)||!strcmp(path,second)){*bytes=4096;return 0;}
 if(!strcmp(path,"bad-sized.a26")){*bytes=1024;return 0;}
 return -1;
}
int main(int argc,char **argv){
 spmp_core_host host={0};host.present=screen;host.buttons=keys;host.read_entire=get_rom;
 host.file_size=file_size;
 if(argc==2){
  private_rom=argv[1];
  if(spmp_core_start("atari2600",&host,private_rom))return 20;
  for(unsigned i=0;i<5;i++)if(spmp_core_tick())return 21;
  spmp_core_stop();if(rendered!=5||errors||reads!=1)return 22;
  puts("PASS: private external Atari ROM, 5 host frames, normal stop; no device/performance claim");return 0;
 }
 if(spmp_core_start("atari2600",&host,first))return 10;
 if(spmp_core_tick()||spmp_core_tick())return 11;
 if(RiotRam[0]!=0x42){fprintf(stderr,"6507 RAM unexpected: %02x PC=%04x\n",RiotRam[0],reg_pc);return 12;}
 if(rendered!=2||errors||Frame<1)return 13;
 spmp_core_stop();
 if(spmp_core_start("atari2600",&host,second))return 14;
 if(spmp_core_tick()||RiotRam[0]!=0x24||rendered!=3)return 15;
 spmp_core_stop();
 if(!spmp_core_start("atari2600",&host,"bad-sized.a26")||reads!=2)return 16;
 if(!spmp_core_start("atari2600",&host,"missing.a26")||reads!=2)return 17;
 puts("Atari 2600: external/renamed ROM paths, 6507/RIOT/TIA, relaunch and file-size preflight OK");
 return 0;
}
