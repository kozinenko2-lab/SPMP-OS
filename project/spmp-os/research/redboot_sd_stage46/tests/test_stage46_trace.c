#include "spmp_stage46_trace.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 static spmp_bootlog log; spmp_log_init(&log,17,0);
 spmp46_trace_context t={&log,123,SPMP_LOG_SD_SCAN,1};
 spmp46_trace_result(&t,SPMP46_OK,52,1,4,0x04);
 spmp46_trace_result(&t,SPMP46_CRC_ERROR,67,2,100,0x200);
 assert(spmp_log_is_valid(&log));
 assert(log.count==4 && log.entries[0].arg0==52);
 assert(log.entries[2].arg0==67 && log.entries[2].arg1==100);
 assert(log.entries[2].level==SPMP_LOG_ERROR);
 assert(log.entries[1].arg0==0x04 && log.entries[3].arg0==0x200);
 puts("Stage46: bootlog trace integration OK");return 0;
}
