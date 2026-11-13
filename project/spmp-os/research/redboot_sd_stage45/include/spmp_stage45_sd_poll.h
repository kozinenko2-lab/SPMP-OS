#ifndef SPMP_STAGE45_SD_POLL_H
#define SPMP_STAGE45_SD_POLL_H
#include <stdint.h>
/* Read-only diagnostic sampler. This library does not know an MMIO address and
   never accesses hardware directly. Only attach a separately proven backend. */
enum spmp45_result {
    SPMP45_REACHED=0,
    SPMP45_LIMIT=1,
    SPMP45_STATUS_ERROR=2,
    SPMP45_READ_ERROR=3,
    SPMP45_CANCELLED=4,
    SPMP45_INVALID=5
};
typedef int (*spmp45_read_status)(void *ctx, uint32_t *value); /* 1=valid */
typedef int (*spmp45_cancel)(void *ctx); /* 1=abort */
typedef void (*spmp45_report)(void *ctx, enum spmp45_result result,
                              uint32_t polls, uint32_t last_status,
                              uint32_t wanted_mask);
typedef struct {
    void *context;
    spmp45_read_status read_status;
    spmp45_cancel cancelled;
    spmp45_report report;
    uint32_t wanted_mask;
    /* caller explicitly chooses number of samples; max matches OEM upper
       bound including the initial check; do not interpret as milliseconds */
    uint32_t max_samples;
} spmp45_poll_io;
typedef struct {
    enum spmp45_result result;
    uint32_t polls;
    uint32_t last_status;
    uint32_t error_status;
    uint32_t matched;
} spmp45_poll_result;
#define SPMP45_OEM_POLL_THRESHOLD 50000u
#define SPMP45_MAX_SAMPLES 50001u
#define SPMP45_OEM_ERROR_BITS 0x00000630u
spmp45_poll_result spmp45_poll_status(const spmp45_poll_io *io);
#endif
