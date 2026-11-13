#ifndef SPMP_TRACE_BRIDGE_H
#define SPMP_TRACE_BRIDGE_H
#include "spmp_bootlog.h"
#include "spmp_dualboot.h"
/* Instrumented Stage42 boot decision policy; wraps platform ops without
 * changing the platform context or flash. Instrumentation must be best effort. */
typedef uint32_t (*spmp_trace_tick)(void *context);
int spmp_trace_boot(const spmp_db_ops *platform,spmp_bootlog *journal,
                    uint8_t *staging,size_t capacity,
                    spmp_trace_tick tick);
#endif
