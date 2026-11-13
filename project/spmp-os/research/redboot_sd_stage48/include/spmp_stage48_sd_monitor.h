#ifndef SPMP_STAGE48_SD_MONITOR_H
#define SPMP_STAGE48_SD_MONITOR_H
/* Observation-only SD command trace for SPMP-OS research. No SD commands,
 * MMIO, DMA, flash operations or filesystem IO. No claims of driver readiness.
 * Caller serializes callbacks and provides monotonic-ish diagnostic ticks.
 */
#include <stdint.h>
#include "spmp_bootlog.h"
#define SPMP48_EVENT_CAP 64u
#define SPMP48_EVENT_BASE 0x4800u
#define SPMP48_FLAG_RESPONSE_OBSERVED 0x01u
#define SPMP48_FLAG_CONTROLLER_STATUS_OBSERVED 0x02u
#define SPMP48_FLAG_HW_CRC_REPORTED_OK 0x04u /* backend report, NOT verified here */
#define SPMP48_ALLOWED_FLAGS 0x07u

enum spmp48_class { SPMP48_NORMAL=0,SPMP48_APP=1,SPMP48_UNCLASSIFIED=2 };
enum spmp48_outcome { SPMP48_SUCCESS=0,SPMP48_TIMEOUT=1,
 SPMP48_ERROR=2,SPMP48_NOT_SUPPORTED=3 };
typedef struct {
 uint32_t sequence,tick,command_word,argument,response0,controller_status;
 uint32_t flags, result, classification;
} spmp48_event;
typedef struct {
 uint32_t magic, head, count, dropped, next_sequence;
 uint32_t total, successes, failures, cmd55_pending, last_command;
 spmp48_event entries[SPMP48_EVENT_CAP];
 spmp_bootlog *optional_bootlog;
 uint32_t detailed;
} spmp48_monitor;

void spmp48_monitor_init(spmp48_monitor *m,spmp_bootlog *log,uint32_t detailed);
/* This reports a command that a *different*, validated backend observed.
 * Never initiates a command and never changes boot selection or OEM fallback.
 * A successful CMD55 followed immediately by CMD6/41 in the trace allows
 * the latter to be classified as ACMD6/41; not a claim about the SD wire.
 */
int spmp48_monitor_observe(spmp48_monitor *m,uint32_t tick,
    uint32_t command_word,uint32_t argument,uint32_t response0,
    uint32_t controller_status,uint32_t flags,enum spmp48_outcome result);
int spmp48_monitor_get(const spmp48_monitor *m,uint32_t chronological_index,spmp48_event *out);
#endif
