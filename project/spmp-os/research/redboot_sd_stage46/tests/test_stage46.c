#include "spmp_stage46_sector_guard.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
 int begin_ok,finish_ok,recover_ok;
 int fail_first_status,fail_second_status,force_cancel;
 uint32_t good_after, samples, begins, finishes, recovers, reports;
 uint32_t data_size,flags,last_lba;
 uint32_t result_code,reported_attempts,reported_polls;
 uint8_t pattern;
} fake;
static int begin(void *ctx,uint32_t lba){
 fake *x=ctx;x->begins++;x->samples=0;x->last_lba=lba;
 return x->begin_ok;
}
static int poll_status(void *ctx,uint32_t *value){
 fake *x=ctx;x->samples++;
 if((x->fail_first_status && x->begins==1) ||
    (x->fail_second_status && x->begins>=2))return 0;
 *value=x->samples>=x->good_after?0x04u:0u;
 return 1;
}
static int cancel_poll(void *ctx){fake *x=ctx;return x->force_cancel;}
static int finish(void *ctx,uint8_t *dst,uint32_t *n,uint32_t *flags){
 fake *x=ctx;x->finishes++;
 memset(dst,x->pattern,SPMP46_SECTOR_BYTES);
 *n=x->data_size;*flags=x->flags;
 return x->finish_ok;
}
static int recover(void *ctx){fake *x=ctx;x->recovers++;return x->recover_ok;}
static void report(void *ctx,enum spmp46_code code,uint32_t lba,
                   uint32_t tries,uint32_t polls,uint32_t raw){
 fake *x=ctx;x->reports++;x->result_code=(uint32_t)code;
 x->reported_attempts=tries;x->reported_polls=polls;
 (void)lba;(void)raw;
}
static void reset(fake *f){
 memset(f,0,sizeof(*f));f->begin_ok=1;f->finish_ok=1;
 f->recover_ok=1;f->good_after=1;f->data_size=512;
 f->flags=SPMP46_REQUIRED_FLAGS;f->pattern=0xa5;
}
static spmp46_reader io(fake *f){
 spmp46_reader r={f,begin,poll_status,cancel_poll,finish,recover,report,4,5,1};
 return r;
}
static void unchanged(const uint8_t *b){
 for(int i=0;i<512;i++)assert(b[i]==0x77);
}
int main(void){
 fake f;spmp46_reader r;spmp46_result res;
 uint8_t data[512];
 reset(&f);r=io(&f);memset(data,0x77,sizeof(data));
 res=spmp46_read_sector(&r,123,data,512);
 assert(res.code==SPMP46_OK && res.attempts_used==1);
 assert(f.begins==1 && f.finishes==1 && f.recovers==0 && f.reports==1);
 assert(f.last_lba==123 && f.reported_polls==1);
 for(int i=0;i<512;i++)assert(data[i]==0xa5);
 /* The output stays intact when CRC is not confirmed, including after retries. */
 reset(&f);f.flags &=~SPMP46_FLAG_CRC_VALID;r=io(&f);r.attempts=3;
 memset(data,0x77,512);res=spmp46_read_sector(&r,11,data,512);
 assert(res.code==SPMP46_CRC_ERROR && res.attempts_used==3);
 assert(f.recovers==3 && f.reports==1);unchanged(data);
 reset(&f);f.good_after=10;r=io(&f);r.attempts=2;r.poll_limit=3;
 memset(data,0x77,512);res=spmp46_read_sector(&r,77,data,512);
 assert(res.code==SPMP46_POLL_TIMEOUT && res.polls_total==6);
 assert(f.finishes==0 && f.recovers==2);unchanged(data);
 reset(&f);f.fail_first_status=1;r=io(&f);r.attempts=2;
 memset(data,0x77,512);res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_OK && res.attempts_used==2);
 assert(f.recovers==1 && f.reports==1);
 reset(&f);f.finish_ok=0;r=io(&f);
 memset(data,0x77,512);res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_DATA_ERROR && f.recovers==1);unchanged(data);
 reset(&f);f.data_size=511;r=io(&f);
 memset(data,0x77,512);res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_DATA_ERROR);unchanged(data);
 reset(&f);f.flags&=~SPMP46_FLAG_CARD_PRESENT;r=io(&f);
 memset(data,0x77,512);res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_DATA_ERROR);unchanged(data);
 reset(&f);f.recover_ok=0;f.good_after=10;r=io(&f);
 memset(data,0x77,512);res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_RECOVERY_ERROR && f.recovers==1);unchanged(data);
 reset(&f);f.begin_ok=0;r=io(&f);
 memset(data,0x77,512);res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_PREPARE_ERROR && f.recovers==0);unchanged(data);
 reset(&f);f.force_cancel=1;r=io(&f);
 memset(data,0x77,512);res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_CANCELLED && res.polls_total==0 && f.recovers==0);unchanged(data);
 reset(&f);r=io(&f);r.poll_limit=50002;
 memset(data,0x77,512);res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_INVALID && f.begins==0);unchanged(data);
 reset(&f);r=io(&f);r.attempts=4;
 res=spmp46_read_sector(&r,5,data,512);
 assert(res.code==SPMP46_INVALID && f.begins==0);
 r=io(&f);res=spmp46_read_sector(&r,5,data,511);
 assert(res.code==SPMP46_INVALID && f.begins==0);
 /* Fuzz deterministic variant settings; compare 10,000 transactions to
    simple invariants: no partial result and no out-of-budget callback. */
 uint32_t seed=0x46badbee;
 for(int i=0;i<10000;i++){
     reset(&f);seed=seed*1664525u+1013904223u;
     f.pattern=(uint8_t)(seed>>20);
     f.good_after=1+(seed%9u);
     f.flags=(seed&0x100u)?SPMP46_REQUIRED_FLAGS:0;
     f.data_size=(seed&0x200u)?512u:511u;
     r=io(&f);r.attempts=1+((seed>>12)%3);
     r.poll_limit=1+((seed>>16)%10);
     memset(data,0x77,512);
     res=spmp46_read_sector(&r,(uint32_t)i,data,512);
     assert(res.attempts_used<=r.attempts);
     assert(res.polls_total<=r.attempts*r.poll_limit);
     assert(f.reports==1);
     if(res.code==SPMP46_OK){
         for(int j=0;j<512;j++) assert(data[j]==f.pattern);
     }else unchanged(data);
 }
 puts("Stage46: 10,000 deterministic transactions + targeted failures OK");
 return 0;
}
