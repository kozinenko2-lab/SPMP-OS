#include "spmp_stage46_trace.h"
void spmp46_trace_result(void *opaque, enum spmp46_code code,
                         uint32_t lba, uint32_t attempts,
                         uint32_t polls, uint32_t status){
    spmp46_trace_context *t=(spmp46_trace_context *)opaque;
    uint16_t level=SPMP_LOG_INFO;
    const char *message="SD: sector read complete";
    if(!t || !t->journal)return;
    if(code!=SPMP46_OK){
        level=code==SPMP46_POLL_TIMEOUT?SPMP_LOG_WARN:SPMP_LOG_ERROR;
        message="SD: guarded sector read failed";
    }
    /* Avoid raw sector bytes, filenames and personal data. Key telemetry
       is lba + result, polls and final status; retries in event low bits. */
    spmp_log_event(t->journal,t->tick,t->phase,level,
                   0x46000000u|((uint32_t)code<<8)|(attempts&0xffu),
                   lba, polls, message);
    if(t->detailed){
        spmp_log_event(t->journal,t->tick,t->phase,
                       code==SPMP46_OK?SPMP_LOG_DEBUG:SPMP_LOG_WARN,
                       0x46010000u | ((uint32_t)code << 8) | (attempts & 0xffu),
                       status, attempts, "SD: final status/attempt count");
    }
}
