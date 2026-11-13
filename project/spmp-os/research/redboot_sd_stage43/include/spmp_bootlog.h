/* SPMP-OS experimental boot trace logger (RedBoot bridge / OEM MMI).
 * No MMIO, SD writes, malloc, libc, interrupt hooks or firmware patching.
 * Requires caller-validated memory ownership and serialized access.
 * v1 journal serialization: ASCII text lines via a callback after FAT mount.
 */
#ifndef SPMP_BOOTLOG_H
#define SPMP_BOOTLOG_H
#include <stddef.h>
#include <stdint.h>

#define SPMP_LOG_MAGIC 0x474c5053u /* 'SPLG' in little endian */
#define SPMP_LOG_VERSION 1u
#ifndef SPMP_LOG_RING_CAP
#define SPMP_LOG_RING_CAP 256u
#endif
#define SPMP_LOG_MSG_BYTES 64u
#define SPMP_LOG_LINE_BYTES 64u

enum spmp_log_phase {
    SPMP_LOG_RED_BOOT = 1, SPMP_LOG_SD_SCAN = 2,
    SPMP_LOG_MMI_BOOT = 3, SPMP_LOG_MMI_FS = 4,
    SPMP_LOG_MMI_UI = 5, SPMP_LOG_SPMP_OS = 6
};
enum spmp_log_level {SPMP_LOG_DEBUG=0, SPMP_LOG_INFO=1,
                     SPMP_LOG_WARN=2, SPMP_LOG_ERROR=3};

typedef struct {
    uint32_t sequence, tick, event, arg0, arg1;
    uint16_t phase, level, msg_len, reserved;
    char message[SPMP_LOG_MSG_BYTES];
    uint32_t checksum;
} spmp_log_record;

typedef struct {
    uint32_t magic, version, session, head, count, next_sequence, dropped;
    spmp_log_record entries[SPMP_LOG_RING_CAP];
    uint16_t line_len, line_phase;
    char pending_line[SPMP_LOG_LINE_BYTES];
} spmp_bootlog;

/* init(preserve=1) reuses ONLY valid data with valid checksums. It cannot
 * preserve logs if the OEM RAP resets or overwrites this RAM area. */
void spmp_log_init(spmp_bootlog *log, uint32_t session, int preserve);
void spmp_log_event(spmp_bootlog *log, uint32_t tick, uint16_t phase,
                    uint16_t level, uint32_t event, uint32_t a, uint32_t b,
                    const char *message);
/* Incremental tap for console putc; do not call from a filesystem callback. */
void spmp_log_console_char(spmp_bootlog *log, uint32_t tick,
                           uint16_t phase, int character);
void spmp_log_console_flush(spmp_bootlog *log, uint32_t tick);

typedef struct {
    void *context;
    /* Return exactly count on success, negative on error. Never block
     * indefinitely: the OEM filesystem adapter must supply a timeout. */
    int (*append)(void *context, const char *buf, size_t count);
    int (*sync)(void *context); /* optional but recommended */
} spmp_log_sink;
/* Caller must have opened a valid log file AFTER verified writable FAT mount.
 * A failed log operation is informational and must NOT block factory boot. */
int spmp_log_export_text(const spmp_bootlog *log, const spmp_log_sink *sink,
                         uint32_t max_records, uint32_t *written_records);
/* Incremental export, for periodic post-mount flushes; header is written
 * each time. Repeated flush after an interrupted partial write may duplicate
 * a few sequence numbers, so consumers should deduplicate by session+seq. */
int spmp_log_export_since(const spmp_bootlog *log, const spmp_log_sink *sink,
                          uint32_t after_sequence, uint32_t max_records,
                          uint32_t *written_records, uint32_t *last_sequence);
int spmp_log_is_valid(const spmp_bootlog *log);
#endif
