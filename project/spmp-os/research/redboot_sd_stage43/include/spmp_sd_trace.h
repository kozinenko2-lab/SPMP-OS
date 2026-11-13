#ifndef SPMP_SD_TRACE_H
#define SPMP_SD_TRACE_H
#include "spmp_bootlog.h"
#include "spmp_fat_reader.h"
/* Read-only sector tracing, not an SD/MMC host controller implementation. */
typedef uint32_t (*spmp_sd_tick)(void *context);
typedef struct {
    spmp_fat_io base;
    spmp_bootlog *journal;
    spmp_sd_tick clock;
    uint32_t sampled_every; /* 1=all, 0=errors only */
    uint32_t reads, failures, reads_since_sample;
} spmp_sd_trace;
/* call spmp_fat_read_boot with the returned IO descriptor */
spmp_fat_io spmp_sd_trace_io(spmp_sd_trace *t);
#endif
