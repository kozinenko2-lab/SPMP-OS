#ifndef SPMP_STAGE46_TRACE_H
#define SPMP_STAGE46_TRACE_H
#include "spmp_stage46_sector_guard.h"
#include "spmp_bootlog.h"
typedef struct {
 spmp_bootlog *journal;
 uint32_t tick;
 uint16_t phase;
 uint16_t detailed; /* emit a second status record for deeper diagnosis */
} spmp46_trace_context;
/* Safe in non-IRQ context, no FAT/filesystem operations. */
void spmp46_trace_result(void *ctx, enum spmp46_code code, uint32_t lba,
                         uint32_t attempts, uint32_t polls, uint32_t status);
#endif
