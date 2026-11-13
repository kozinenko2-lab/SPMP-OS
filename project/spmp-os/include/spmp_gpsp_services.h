#ifndef SPMP_GPSP_SERVICES_H
#define SPMP_GPSP_SERVICES_H
/* gpSP ARM926 freestanding runtime adapters; no MMIO addresses are assumed. */
#include <stddef.h>
#include <stdint.h>
typedef void (*spmp_gpsp_log_sink)(void *ctx, const char *data, size_t length);
void spmp_gpsp_bind_log(spmp_gpsp_log_sink sink, void *ctx);
/* Return nonzero and fill UTC Unix seconds iff the platform clock is valid. */
typedef int (*spmp_gpsp_clock_read)(void *ctx, int64_t *epoch);
void spmp_gpsp_bind_clock(spmp_gpsp_clock_read clock, void *ctx);
/* Fallback is an explicit deterministic virtual clock at 2000-01-01 UTC. */
int spmp_gpsp_set_virtual_epoch(int64_t epoch);
int spmp_gpsp_set_utc_offset_minutes(int offset_minutes);
#endif
