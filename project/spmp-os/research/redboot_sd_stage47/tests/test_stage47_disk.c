/* Real FAT parser + Stage46 guard + Stage41 policy, but host FILE disk only.
 * Disk file MUST be a synthetic fixture; no device nodes or real disks. */
#include "spmp_stage47_guarded_fat.h"
#include "spmp_dualboot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

typedef struct {spmp47_guarded_fat f;FILE*image;int mode,sd,stock,restore;
    uint32_t pending_lba;int card,force,reads; } Env;
static jmp_buf handoff;
static int begin(void*p,uint32_t lba){Env*e=p;e->pending_lba=lba;e->reads++;return e->card && e->mode!=1;}
static int stat(void*p,uint32_t*v){Env*e=p;if(e->mode==2)return 0;*v=4;return 1;}
static int finish(void*p,uint8_t*out,uint32_t*n,uint32_t*flags){Env*e=p;
 if(e->mode==3)return 0;
 if(fseek(e->image,(long)e->pending_lba*512,SEEK_SET)||fread(out,1,512,e->image)!=512)return 0;
 *n=512;*flags=e->mode==4?(SPMP46_REQUIRED_FLAGS&~SPMP46_FLAG_CRC_VALID):SPMP46_REQUIRED_FLAGS;
 return 1;
}
static int recover(void*p){(void)p;return 1;}
static int force(void*p){return ((Env*)p)->force;}
static int restore(void*p){((Env*)p)->restore++;return 1;}
static int preflight(void*p,const uint8_t*b,size_t n,uint32_t*entry){(void)p;(void)b;
 if(n<52)return 0;*entry=0x40000;return 1;}
static void jump(void*p,uint32_t entry){Env*e=p;
 if(entry!=0x40000)abort(); e->sd++;if(e->mode!=5)longjmp(handoff,1);}
static void stock(void*p){((Env*)p)->stock++;}
static int rd(void*p,const char*path,uint8_t*b,size_t cap,size_t*n){Env*e=p;
 return spmp47_read_package(&e->f,path,b,cap,n);}
int main(int argc,char **argv){
 static uint8_t staging[8192];Env e={0};spmp46_reader r={0};spmp_db_ops ops={0};
 spmp_bootlog log;long length;int expect_sd,expect_stock,expect_reads;
 if(argc!=5){fprintf(stderr,"usage: img mode expect_sd expect_stock\n");return 2;}
 if(strstr(argv[1],"/dev/")||strstr(argv[1],"/proc/"))return 2;
 e.image=fopen(argv[1],"rb");if(!e.image)return 2;
 if(fseek(e.image,0,SEEK_END))return 2;length=ftell(e.image);
 if(length<512 || length%512)return 2;
 e.mode=atoi(argv[2]);expect_sd=atoi(argv[3]);expect_stock=atoi(argv[4]);
 e.card=e.mode!=6;e.force=e.mode==7;expect_reads=!e.force;
 r.context=&e;r.begin_read=begin;r.read_status=stat;
 r.finish_read=finish;r.recover=recover;r.completion_mask=4;r.poll_limit=3;r.attempts=2;
 spmp_log_init(&log,47,0);
 if(!spmp47_guarded_fat_init(&e.f,&r,(uint32_t)(length/512),64,&log,1))return 2;
 ops.context=&e;ops.verified_board_id=0x00420001;ops.force_factory=force;
 ops.read_package=rd;ops.restore_factory_environment=restore;
 ops.preflight_elf=preflight;ops.stage_elf=preflight;
 ops.jump_sd=jump;ops.boot_factory=stock;
 if(!setjmp(handoff)){
  if(spmp_db_boot(&ops,staging,sizeof(staging))!=-2)return 3;
 }
 if(e.sd!=expect_sd || e.stock!=expect_stock || !spmp_log_is_valid(&log)||
    (expect_reads&&e.reads==0) || (!expect_reads&&e.reads!=0)){
     fprintf(stderr,"BAD mode%d: sd%d stock%d reads%d\n",e.mode,e.sd,e.stock,e.reads);return 1;
 }
 fclose(e.image);
 printf("PASS mode=%d sd=%d stock=%d sectors=%d\n",e.mode,e.sd,e.stock,e.reads);
 return 0;
}
