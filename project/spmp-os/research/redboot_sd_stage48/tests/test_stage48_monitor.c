#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "spmp_stage48_sd_monitor.h"
static uint32_t prng(uint32_t *v){*v=*v*1664525u+1013904223u;return *v;}
int main(void){
 spmp48_monitor m;spmp48_event e;spmp_bootlog log;
 uint32_t seed=0x48624u;
 unsigned i;
 spmp_log_init(&log,0x48u,0);
 spmp48_monitor_init(&m,&log,0);
 assert(m.count==0 && !spmp48_monitor_get(&m,0,&e));
 assert(spmp48_monitor_observe(&m,1,0x10b7u,0,0,0,0,SPMP48_SUCCESS));
 assert(spmp48_monitor_observe(&m,2,0x30a9u,1,2,3,SPMP48_FLAG_RESPONSE_OBSERVED,SPMP48_SUCCESS));
 assert(spmp48_monitor_get(&m,1,&e));
 assert(e.classification==SPMP48_APP && e.command_word==0x30a9u);
 assert(e.response0==2 && e.controller_status==0);
 assert(spmp48_monitor_observe(&m,3,6,7,8,9,0,SPMP48_TIMEOUT));
 assert(spmp48_monitor_get(&m,2,&e) && e.classification==SPMP48_UNCLASSIFIED);
 assert(log.count==1); /* only error logged */
 assert(!spmp48_monitor_observe(&m,4,12,0,0,0,8,SPMP48_SUCCESS));
 assert(m.count==3 && m.total==3);
 assert(!spmp48_monitor_observe(&m,5,12,0,0,0,0,(enum spmp48_outcome)99));
 for(i=0;i<100000;i++){
    uint32_t v=prng(&seed),w=v & 0x3ffffu;
    enum spmp48_outcome r=(enum spmp48_outcome)((v>>24)&3u);
    assert(spmp48_monitor_observe(&m,i+100,w,v,v,v,SPMP48_ALLOWED_FLAGS,r));
    if((i%17u)==0){assert(spmp48_monitor_get(&m,m.count-1,&e));assert(e.command_word==w);}
 }
 assert(m.count==SPMP48_EVENT_CAP && m.dropped==100003u-SPMP48_EVENT_CAP);
 assert(m.total==100003u);
 assert(m.successes+m.failures==m.total);
 assert(spmp_log_is_valid(&log));
 printf("stage48: 100000 bounded observation operations + RAM bootlog OK\n");
 return 0;
}
