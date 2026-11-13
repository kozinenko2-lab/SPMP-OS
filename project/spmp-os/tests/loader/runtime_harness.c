#include "spmp_runtime.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
static unsigned saves;static int reject_save;
static int save(void *ctx,const char *core,const char *id){
 (void)ctx;assert(core&&*core&&id&&*id);saves++;
 return reject_save?-1:0;
}
static uint8_t *readall(const char *path,size_t *n){
 FILE *f=fopen(path,"rb");assert(f);assert(!fseek(f,0,SEEK_END));long v=ftell(f);assert(v>0&&v<65536);rewind(f);
 uint8_t *b=malloc((size_t)v);assert(b);assert(fread(b,1,(size_t)v,f)==(size_t)v);fclose(f);*n=(size_t)v;return b;
}
int main(int argc,char **argv){
 assert(argc==3);size_t a_len,b_len;
 uint8_t *a=readall(argv[1],&a_len),*b=readall(argv[2],&b_len);
 uint8_t code[16*1024],game[128*1024];memset(code,0xcc,sizeof code);memset(game,0xcc,sizeof game);
 spmp_overlay_options opt={.virtual_base=0x20000000u};
 spmp_runtime rt;spmp_runtime_init(&rt,code,sizeof code,game,sizeof game,&opt,save,NULL);
 uint8_t rom1[128],rom2[64];memset(rom1,0x11,sizeof rom1);memset(rom2,0x22,sizeof rom2);
 assert(spmp_runtime_launch(&rt,a,a_len,"ROM A",rom1,sizeof rom1)==0);
 assert(rt.state==SPMP_RUN_GAME&&rt.loader.generations==1&&rt.rom_changes==1&&saves==0);
 assert(spmp_runtime_pause(&rt)==0 && rt.state==SPMP_RUN_PAUSED);
 assert(spmp_runtime_launch(&rt,a,a_len,"ROM A",rom1,sizeof rom1)==1);
 assert(rt.state==SPMP_RUN_GAME&&rt.rom_changes==1&&saves==0);
 /* Same core, different ROM: save once and reuse code. */
 assert(spmp_runtime_launch(&rt,a,a_len,"ROM B",rom2,sizeof rom2)==0);
 assert(rt.loader.generations==1&&rt.rom_changes==2&&saves==1);
 assert(rt.game.memory[0]==0x22&&rt.game_id[4]=='B');
 /* Oversize/invalid core cannot destroy current game. */
 uint8_t *oversize=malloc(100000);assert(oversize);
 assert(spmp_runtime_launch(&rt,a,a_len,"Too big",oversize,100000)==SPMP_OV_NO_MEMORY);
 free(oversize);
 uint8_t *bad=malloc(a_len);assert(bad);memcpy(bad,a,a_len);bad[18]=0x3e;
 assert(spmp_runtime_launch(&rt,bad,a_len,"bad",rom1,sizeof rom1)==SPMP_OV_BAD_ELF);
 assert(rt.rom_changes==2&&rt.game.memory[0]==0x22);
 reject_save=1;
 assert(spmp_runtime_launch(&rt,b,b_len,"other",rom1,sizeof rom1)==SPMP_OV_NOT_READY);
 assert(rt.loader.generations==1&&rt.state==SPMP_RUN_GAME);
 reject_save=0;
 assert(spmp_runtime_launch(&rt,b,b_len,"other",rom1,sizeof rom1)==0);
 assert(rt.loader.generations==2&&rt.rom_changes==3&&rt.game.memory[0]==0x11);
 assert(spmp_runtime_stop_game(&rt)==0);
 assert(rt.state==SPMP_RUN_CORE_READY&&rt.loader.loaded&&rt.game.used==0);
 assert(spmp_runtime_shutdown(&rt)==0 && !rt.loader.loaded&&rt.state==SPMP_RUN_EMPTY);
 assert(saves==4); /* saves: A->B, rejected attempt, B->other, stop */
 free(a);free(b);free(bad);
 puts("Stage10 runtime: pause/resume, ROM-only switching, cross-core switch, save failure, RAM release OK");
 return 0;
}
