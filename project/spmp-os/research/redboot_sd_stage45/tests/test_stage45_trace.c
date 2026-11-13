#include "spmp_stage45_trace.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    static spmp_bootlog log;
    spmp45_trace_context t={&log,1234,SPMP_LOG_SD_SCAN};
    spmp_log_init(&log,0x45000001u,0);
    spmp45_trace_poll_result(&t,SPMP45_LIMIT,50001u,0x4,0x40);
    assert(spmp_log_is_valid(&log));
    assert(log.count==1);
    assert(log.entries[0].arg0==50001u);
    assert(log.entries[0].arg1==0x4);
    assert(log.entries[0].event==0x45010040u);
    assert(log.entries[0].level==SPMP_LOG_WARN);
    spmp45_trace_poll_result(&t,SPMP45_REACHED,6u,0x40,0x40);
    assert(log.count==2);
    assert(log.entries[1].level==SPMP_LOG_DEBUG);
    assert(spmp_log_is_valid(&log));
    puts("Stage45 logger hook passed: 2 distinct status events");
    return 0;
}
