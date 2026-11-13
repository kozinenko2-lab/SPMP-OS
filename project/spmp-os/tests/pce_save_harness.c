/* In-memory per-game battery RAM tests, no filesystem writes. */
#include "spmp_cores.h"
#include "shared.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
extern uint8 save_bram;
static uint8_t card[8192], save_a[8192];
static unsigned presents,writes,loads;
static int have_a;
static void present(void *u,const uint16_t *buf,unsigned w,unsigned h){
 (void)u;if(buf&&w==320&&h==240)presents++;
}
static int read_rom(void *u,const char *game,uint8_t *dst,size_t cap,size_t *out){
 (void)u;
 if(!game || (strcmp(game,"a.pce")&&strcmp(game,"b.pce")) ||cap<sizeof card)return -1;
 memcpy(dst,card,sizeof card);*out=sizeof card;return 0;
}
static int load(void *u,const char *id,const char *game,uint8_t *dst,size_t cap,size_t *got){
 (void)u;loads++;
 if(strcmp(id,"pcengine")||cap<sizeof save_a||strcmp(game,"a.pce")||!have_a)return -1;
 memcpy(dst,save_a,sizeof save_a);*got=sizeof save_a;return 0;
}
static int store(void *u,const char *id,const char *game,const uint8_t *data,size_t len){
 (void)u;
 if(strcmp(id,"pcengine")||strcmp(game,"a.pce")||len!=sizeof save_a)return -1;
 memcpy(save_a,data,len);writes++;have_a=1;return 0;
}
int main(void){
 memset(card,0xea,sizeof card);card[0x1ffe]=0;card[0x1fff]=0xe0;
 spmp_core_host host={0};host.present=present;host.read_entire=read_rom;
 host.load_save=load;host.store_save=store;
 if(spmp_core_start("pcengine",&host,"a.pce"))return 1;
 if(bram[0]!=0)return 2;
 /* save test does not require stepping CPU (covered by pcengine_harness). */
 spmp_core_stop();
 if(writes)return 4; /* no persistence without BRAM write latch */
 if(spmp_core_start("pcengine",&host,"a.pce"))return 5;
 bram[0]=0xa5;save_bram=1; /* emulate a cartridge accessing backup RAM */
 spmp_core_stop();
 if(!have_a||save_a[0]!=0xa5||writes!=1)return 6;
 if(spmp_core_start("pcengine",&host,"a.pce"))return 7;
 if(bram[0]!=0xa5)return 8;
 spmp_core_stop();
 if(spmp_core_start("pcengine",&host,"b.pce"))return 9;
 if(bram[0]!=0)return 10; /* different game does not inherit A */
 spmp_core_stop();
 if(writes!=1||loads<4)return 11;
 puts("PC Engine per-ROM BRAM: isolation, load, dirty write, no implicit disk writes OK");
 return 0;
}
