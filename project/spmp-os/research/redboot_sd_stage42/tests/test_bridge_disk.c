#include "spmp_dualboot.h"
#include "spmp_fat_bridge.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    spmp_fat_bridge_context bridge;FILE *f;
    int force, fail, jump_return, sd, stock, restore, read_calls;
} Env;
static jmp_buf done;
static int sector(void*ctx,uint32_t lba,uint8_t out[512]){
    Env*e=ctx;e->read_calls++;if(e->fail)return 0;
    return !fseek(e->f,(long)lba*512,SEEK_SET)&&fread(out,1,512,e->f)==512;
}
static int read_pkg(void*ctx,const char*path,uint8_t*dst,size_t cap,size_t*n){
    Env*e=ctx;return spmp_fat_bridge_read_package(&e->bridge,path,dst,cap,n);
}
static int force_factory(void*ctx){return ((Env*)ctx)->force;}
static int restore(void*ctx){((Env*)ctx)->restore++;return 1;}
static int preflight(void*ctx,const uint8_t*elf,size_t n,uint32_t*entry){
    (void)ctx;(void)elf;if(n<52)return 0;*entry=0x40000;return 1;
}
static int stage_elf(void*ctx,const uint8_t*elf,size_t n,uint32_t*entry){
    (void)ctx;(void)elf;if(n<52)return 0;*entry=0x40000;return 1;
}
static void jump_sd(void*ctx,uint32_t entry){
    Env*e=ctx;if(entry!=0x40000)abort();e->sd++;
    if(!e->jump_return)longjmp(done,1);
}
static void stock(void*ctx){((Env*)ctx)->stock++;}
int main(int argc,char**argv){
    static uint8_t stage[8192];Env e={0};spmp_db_ops ops={0};long sz;
    int expected_sd,expected_read,returned;
    if(argc!=5){fprintf(stderr,"usage: bridge disk.img expect_sd force jump_return\n");return 2;}
    e.f=fopen(argv[1],"rb");if(!e.f)return 2;
    fseek(e.f,0,SEEK_END);sz=ftell(e.f);
    if(sz<512||sz%512)return 2;
    e.bridge.io=(spmp_fat_io){.context=&e,.physical_sectors=(uint32_t)(sz/512),
      .max_reads=128,.read_sector=sector};
    expected_sd=atoi(argv[2]);e.force=atoi(argv[3]);e.jump_return=atoi(argv[4]);
    ops.context=&e;ops.verified_board_id=0x00420001;
    ops.force_factory=force_factory;ops.read_package=read_pkg;
    ops.restore_factory_environment=restore;
    ops.preflight_elf=preflight;ops.stage_elf=stage_elf;
    ops.jump_sd=jump_sd;ops.boot_factory=stock;
    returned=!setjmp(done);
    if(returned){int rc=spmp_db_boot(&ops,stage,sizeof(stage));if(rc!=-2)return 3;}
    expected_read=!e.force;
    if(e.sd!=(expected_sd?1:0)||e.stock!=(expected_sd==1?0:1)||
       (expected_read&&e.read_calls<1)||(!expected_read&&e.read_calls>0)||
       e.restore!=(expected_sd==1?0:expected_read)){
        fprintf(stderr,"FAIL sd=%d stock=%d restore=%d reads=%d\n",e.sd,e.stock,e.restore,e.read_calls);
        return 1;
    }
    printf("PASS SD=%d factory=%d read=%d restore=%d\n",e.sd,e.stock,e.read_calls,e.restore);
    fclose(e.f);return 0;
}
