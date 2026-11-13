#include "spmp_stage47_guarded_fat.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct { int fail,reads,recovery,report; uint32_t lba; } Mock;
static int begin(void *p,uint32_t lba){Mock *m=p;m->lba=lba;return 1;}
static int status(void *p,uint32_t *v){Mock*m=p; m->reads++;if(m->fail==1)return 0;*v=4;return 1;}
static int finish(void *p,uint8_t *b,uint32_t *n,uint32_t *flags){Mock*m=p;
 if(m->fail==2) return 0;
 memset(b,(int)(m->lba&255),512);*n=512;
 *flags=m->fail==3?(SPMP46_REQUIRED_FLAGS&~SPMP46_FLAG_CRC_VALID):SPMP46_REQUIRED_FLAGS;
 return 1;
}
static int recover(void *p){Mock*m=p;m->recovery++;return 1;}
static void report(void *p,enum spmp46_code c,uint32_t l,uint32_t a,uint32_t polls,uint32_t statusval){
 Mock*m=p;(void)c;(void)l;(void)a;(void)polls;(void)statusval;m->report++;
}
int main(void){
 spmp47_guarded_fat f;spmp46_reader r={0};spmp_bootlog log;
 uint8_t b[512];uint32_t arg;Mock m={0};int i;
 assert(!spmp47_command_argument(1,SPMP47_ADDR_BYTES,0));
 assert(spmp47_command_argument(123,SPMP47_ADDR_BYTES,&arg)&&arg==62976);
 assert(spmp47_command_argument(UINT32_MAX/512u,SPMP47_ADDR_BYTES,&arg)&&arg==(UINT32_MAX/512u)*512u);
 assert(!spmp47_command_argument(UINT32_MAX/512u+1u,SPMP47_ADDR_BYTES,&arg)&&arg==0);
 assert(spmp47_command_argument(UINT32_MAX,SPMP47_ADDR_BLOCKS,&arg)&&arg==UINT32_MAX);
 assert(!spmp47_command_argument(2,(enum spmp47_address_mode)4,&arg)&&arg==0);
 assert(!spmp47_guarded_fat_init(&f,0,100,100,0,0));
 assert(f.bridge.io.read_sector==0);
 r.context=&m;r.begin_read=begin;r.read_status=status;r.finish_read=finish;
 r.recover=recover;r.report=report;r.completion_mask=4;r.poll_limit=5;r.attempts=1;
 spmp_log_init(&log,47,0);
 assert(spmp47_guarded_fat_init(&f,&r,100,100,&log,1));
 for(i=0;i<10000;i++){
   uint32_t lba=(uint32_t)(i%100);
   m.fail=(i%11==0)?1:((i%13==0)?2:((i%17==0)?3:0));
   memset(b,0xa5,sizeof(b));
   int ok=spmp47_guarded_sector(&f,lba,b);
   assert(ok==(m.fail==0));
   for(int j=0;j<512;j++)assert(b[j]==(ok?(uint8_t)lba:0xa5));
 }
 memset(b,0x5a,sizeof(b));
 assert(!spmp47_guarded_sector(&f,100,b));
 assert(b[0]==0x5a); assert(f.successful_sectors>0 && f.failed_sectors>0);
 assert(m.report==10000);assert(spmp_log_is_valid(&log));
 printf("Stage47 guard: 10000 transactions + addressing + fail-closed PASS\n");return 0;
}
