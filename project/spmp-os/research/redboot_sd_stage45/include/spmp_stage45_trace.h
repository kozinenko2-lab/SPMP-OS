#ifndef SPMP_STAGE45_TRACE_H
#define SPMP_STAGE45_TRACE_H
#include "spmp_bootlog.h"
#include "spmp_stage45_sd_poll.h"
typedef struct {
    spmp_bootlog *journal;
    uint32_t monotonic_tick; /* Provided by caller; do not assume milliseconds. */
    uint16_t phase;
} spmp45_trace_context;
/* Callback for spmp45_poll_io.report; this only writes to an existing RAM ring.
 * It does not invoke UART, FAT, interrupts, or controller registers.
 */
void spmp45_trace_poll_result(void *opaque,enum spmp45_result result,
                               uint32_t polls,uint32_t last_status,
                               uint32_t wanted_mask);
#endif
