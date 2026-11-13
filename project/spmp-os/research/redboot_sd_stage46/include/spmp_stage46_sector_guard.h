/* SPMP-OS Stage46: read-only guarded 512-byte transaction for a PROVEN backend.
 * Does not program SD hardware. A caller must implement and validate all IO.
 * Never run this as a RedBoot hook without a separately verified SD controller.
 */
#ifndef SPMP_STAGE46_SECTOR_GUARD_H
#define SPMP_STAGE46_SECTOR_GUARD_H
#include <stdint.h>
#include "spmp_stage45_sd_poll.h"
#define SPMP46_SECTOR_BYTES 512u
#define SPMP46_MAX_ATTEMPTS 3u
#define SPMP46_FLAG_CRC_VALID 1u
#define SPMP46_FLAG_TRANSFER_DONE 2u
#define SPMP46_FLAG_CARD_PRESENT 4u
#define SPMP46_REQUIRED_FLAGS (SPMP46_FLAG_CRC_VALID | SPMP46_FLAG_TRANSFER_DONE | SPMP46_FLAG_CARD_PRESENT)

enum spmp46_code {
 SPMP46_OK=0, SPMP46_INVALID=1, SPMP46_PREPARE_ERROR=2,
 SPMP46_POLL_TIMEOUT=3, SPMP46_STATUS_ERROR=4, SPMP46_STATUS_IO_ERROR=5,
 SPMP46_CANCELLED=6, SPMP46_DATA_ERROR=7, SPMP46_CRC_ERROR=8,
 SPMP46_RECOVERY_ERROR=9
};

typedef struct {
 void *context;
 /* Nonzero return means function succeeded; no implicit retries. */
 int (*begin_read)(void *context, uint32_t lba);
 /* Exactly the Stage45 read_status/cancel callbacks; 1 means valid. */
 spmp45_read_status read_status;
 spmp45_cancel cancelled;
 /* Populate 512-byte temporary RAM. Must set written and flags explicitly;
  * FLAG_CRC_VALID requires a trusted backend's hardware CRC completion.
  * Do not set it based on checking a buffer or computing a software CRC. */
 int (*finish_read)(void *context, uint8_t *staging,
                    uint32_t *written, uint32_t *flags);
 /* Stop/reset pending controller state before next attempt or OEM fallback.
  * Required for every failure after successful begin_read. */
 int (*recover)(void *context);
 /* Called once after transaction; logging only, no filesystem IO. */
 void (*report)(void *context, enum spmp46_code code,
                uint32_t lba, uint32_t attempts, uint32_t polls,
                uint32_t raw_status);
 uint32_t completion_mask; /* nonzero, derived from verified backend */
 uint32_t poll_limit; /* 1..50001 samples, NOT a time in milliseconds */
 uint32_t attempts; /* 1..3, retries only after successful recovery */
} spmp46_reader;

typedef struct {
 enum spmp46_code code;
 uint32_t attempts_used, polls_total, last_status, last_flags;
} spmp46_result;
/* Destination is unchanged on any error. Exact size (512) required.
 * There is no cache maintenance here: the verified backend owns DMA
 * coherency. No writes to flash, SD or MMIO originate from this module. */
spmp46_result spmp46_read_sector(const spmp46_reader *reader,
                                 uint32_t lba, uint8_t *destination,
                                 uint32_t capacity);
#endif
