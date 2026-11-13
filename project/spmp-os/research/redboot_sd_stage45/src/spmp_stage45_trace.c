#include "spmp_stage45_trace.h"
void spmp45_trace_poll_result(void *opaque,enum spmp45_result result,
                               uint32_t polls,uint32_t last_status,
                               uint32_t wanted_mask){
    spmp45_trace_context *trace=(spmp45_trace_context *)opaque;
    uint16_t level;
    const char *text;
    if(!trace || !trace->journal)return;
    switch(result){
    case SPMP45_REACHED: level=SPMP_LOG_DEBUG;text="sd status: completion observed";break;
    case SPMP45_LIMIT: level=SPMP_LOG_WARN;text="sd status: sample budget exhausted";break;
    case SPMP45_STATUS_ERROR: level=SPMP_LOG_ERROR;text="sd status: OEM error bits";break;
    case SPMP45_READ_ERROR: level=SPMP_LOG_ERROR;text="sd status: backend unavailable";break;
    case SPMP45_CANCELLED: level=SPMP_LOG_WARN;text="sd status: cancelled";break;
    default: level=SPMP_LOG_ERROR;text="sd status: invalid poll";break;
    }
    /* The record's event code encodes result and mask, while both arguments
     * retain the number of samples and last raw status. */
    spmp_log_event(trace->journal,trace->monotonic_tick,trace->phase,level,
                    0x45000000u|((uint32_t)result<<16)|(wanted_mask&0xffffu),
                    polls,last_status,text);
}
