/* Deterministic Atari 2600 test, no copyrighted ROMs. */
#include "spmp_cores.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
extern unsigned char RiotRam[128];
extern unsigned int reg_pc;
extern int Frame;
static unsigned rendered,errors;
static void screen(void *user,const uint16_t *pixels,unsigned w,unsigned h){
 (void)user;
 if(!pixels||w!=320||h!=240)errors++;
 rendered++;
}
static uint32_t keys(void *user){(void)user;return 0;}
static int get_rom(void *user,const char *path,uint8_t *dst,size_t cap,size_t *got){
 (void)user;
 if(strcmp(path,"synthetic.a26") || cap<4096)return -1;
 memset(dst,0xea,4096);
 const uint8_t code[]={0xa9,0x42,0x85,0x80,0x4c,0x04,0xf0};
 memcpy(dst,code,sizeof code);
 dst[0xffc]=0x00;dst[0xffd]=0xf0;dst[0xffe]=0x00;dst[0xfff]=0xf0;
 *got=4096;return 0;
}
int main(void){
 spmp_core_host host={0};host.present=screen;host.buttons=keys;host.read_entire=get_rom;
 if(spmp_core_start("atari2600",&host,"synthetic.a26"))return 10;
 if(spmp_core_tick()||spmp_core_tick())return 11;
 if(RiotRam[0]!=0x42){fprintf(stderr,"6507 RAM unexpected: %02x PC=%04x\n",RiotRam[0],reg_pc);return 12;}
 if(rendered!=2||errors||Frame<1)return 13;
 spmp_core_stop();
 if(spmp_core_start("atari2600",&host,"synthetic.a26"))return 14;
 if(spmp_core_tick()||RiotRam[0]!=0x42||rendered!=3)return 15;
 spmp_core_stop();
 puts("Atari 2600: 6507 LDA/STA, RIOT RAM, TIA frames, relaunch OK");
 return 0;
}
