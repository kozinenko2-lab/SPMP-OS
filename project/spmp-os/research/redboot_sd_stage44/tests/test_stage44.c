#include "spmp_stage44_handoff.h"
#include "spmp_stage44_flush.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef union {uint32_t align;unsigned char bytes[SPMP_HANDOFF_SIZE];} aligned_region;
static spmp_bootlog a,b;
static aligned_region region;
static unsigned long checks;
static void check(int c){assert(c);checks++;}
static void handoff(void){
    spmp_log_init(&a,0x130d2026u,0);
    for(unsigned i=0;i<50;i++)spmp_log_event(&a,i,SPMP_LOG_RED_BOOT,SPMP_LOG_INFO,i,7,8,"RedBoot state");
    check(spmp_log_is_valid(&a));
    check(!spmp_stage44_handoff_pack(region.bytes,sizeof(region),&a,8,0));
    check(spmp_stage44_handoff_pack(region.bytes,sizeof(region),&a,8,1));
    check(!spmp_stage44_handoff_unpack(region.bytes,sizeof(region),&b,a.session,7,1));
    check(!spmp_stage44_handoff_unpack(region.bytes,sizeof(region),&b,a.session,8,0));
    check(spmp_stage44_handoff_unpack(region.bytes,sizeof(region),&b,a.session,8,1));
    check(memcmp(&a,&b,sizeof(a))==0);
    check(!spmp_stage44_handoff_unpack(region.bytes,sizeof(region)-1,&b,a.session,8,1));
    for(unsigned i=0;i<10000;i++){
        size_t pos=sizeof(spmp_stage44_handoff_header)+(i*47u)%sizeof(a);
        unsigned char old=region.bytes[pos];
        region.bytes[pos]^=(unsigned char)(1u<<(i&7u));
        check(!spmp_stage44_handoff_unpack(region.bytes,sizeof(region),&b,a.session,8,1));
        region.bytes[pos]=old;
    }
    check(spmp_stage44_handoff_unpack(region.bytes,sizeof(region),&b,a.session,8,1));
    ((spmp_stage44_handoff_header *)region.bytes)->commit=0;
    check(!spmp_stage44_handoff_unpack(region.bytes,sizeof(region),&b,a.session,8,1));
}
typedef struct {
    char data[80000]; size_t used;
    int is_ready,fail_open,fail_write,fail_close,open_calls,close_calls;
} mock_fs;
static int ready(void *ctx){return ((mock_fs*)ctx)->is_ready;}
static int open_append(void *ctx,const char *path){
    mock_fs *m=ctx;m->open_calls++;
    if(strcmp(path,"SPMPOS/LOGS/130D2026.LOG")!=0)return 0;
    return !m->fail_open;
}
static int write_log(void *ctx,const char *data,size_t n){
    mock_fs *m=ctx;
    if(m->fail_write || n>sizeof(m->data)-m->used)return -1;
    memcpy(m->data+m->used,data,n);m->used+=n;return (int)n;
}
static int close_checked(void *ctx){mock_fs *m=ctx;m->close_calls++;return !m->fail_close;}
static void flush_tests(void){
    mock_fs m={0};spmp_stage44_flush_state state={0};
    spmp_stage44_fs ops={&m,ready,open_append,write_log,close_checked};
    check(spmp_stage44_flush(&a,&state,&ops,10)==0);
    check(m.open_calls==0);
    m.is_ready=1;
    m.fail_open=1;
    check(spmp_stage44_flush(&a,&state,&ops,10)==-1);
    check(state.acknowledged_sequence==0);
    m.fail_open=0;m.fail_write=1;
    check(spmp_stage44_flush(&a,&state,&ops,10)==-1);
    check(state.acknowledged_sequence==0);
    m.fail_write=0;m.fail_close=1;
    check(spmp_stage44_flush(&a,&state,&ops,10)==-1);
    check(state.acknowledged_sequence==0);
    m.fail_close=0;
    check(spmp_stage44_flush(&a,&state,&ops,10)==1);
    check(state.acknowledged_sequence==10);
    check(spmp_stage44_flush(&a,&state,&ops,99)==1); /* capped to 32 */
    check(state.acknowledged_sequence==42);
    check(spmp_stage44_flush(&a,&state,&ops,32)==1);
    check(state.acknowledged_sequence==50);
    check(spmp_stage44_flush(&a,&state,&ops,32)==0);
    check(state.successful_closes==3);
    check(strstr(m.data,"seq=50")!=0);
    check(state.active==0);
}
int main(void){handoff();flush_tests();printf("Stage44 test OK; assertions=%lu\n",checks);return 0;}
