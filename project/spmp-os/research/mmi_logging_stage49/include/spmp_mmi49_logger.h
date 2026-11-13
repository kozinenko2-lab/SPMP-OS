#ifndef SPMP_MMI49_LOGGER_H
#define SPMP_MMI49_LOGGER_H
#include <stdint.h>
#include <stddef.h>
/* One session; no heap and no filesystem access in diagnostic callback.
 * The caller must serialize producer/flush (eCos IRQ protection or dedicated task).
 */
#define SPMP_MMI49_CAPACITY 8192u
#define SPMP_MMI49_MAGIC 0x39344D53u

typedef struct {
    uint32_t magic;
    uint32_t used;
    uint32_t acknowledged;
    uint32_t dropped;
    uint32_t failed_flushes;
    uint32_t successful_closes;
    uint8_t enabled;
    uint8_t busy;
    char bytes[SPMP_MMI49_CAPACITY];
} spmp_mmi49_logger;

typedef struct {
    void *ctx;
    /* 1 only if removable SD is verified: mount plus user-prepared
     * SPMPOS.ID marker; pathname alone does NOT prove removable media. */
    int (*sd_ready)(void *ctx);
    /* 1 on open, file descriptor owned by backend. No NAND target allowed. */
    int (*open_append)(void *ctx, const char *path);
    /* Must return exactly n on success. Partial write is failure. */
    int (*write)(void *ctx, const char *data, size_t n);
    /* 1 on successful close, otherwise failure. */
    int (*close_checked)(void *ctx);
} spmp_mmi49_fs;

void spmp_mmi49_init(spmp_mmi49_logger *l, int enable);
/* Existing diagnostic output is forwarded by original caller; this only captures. */
void spmp_mmi49_capture_char(spmp_mmi49_logger *l, int ch);
void spmp_mmi49_capture_text(spmp_mmi49_logger *l, const char *s);
/* Short structured milestone, e.g. MOUNT_OK. No clock or sprintf required. */
void spmp_mmi49_event(spmp_mmi49_logger *l, uint32_t tick, const char *tag, uint32_t value);
/* Optional use from verified diagnostic putc hook: always calls original output. */
void spmp_mmi49_forward(spmp_mmi49_logger *l, void (*original)(void *, int), void *ctx, int ch);
/* Called only from normal post-mount task (NEVER from diag putc / ISR).
 *  1 = committed at least one chunk, 0 = not ready/no work, -1 = failure.
 * On failure caller must continue normal MMI boot. A previous partial
 * write may create a duplicate after retry, so log consumers should dedupe.
 * No fsync: OEM fatfs fsync is a no-op. */
int spmp_mmi49_flush(spmp_mmi49_logger *l, const spmp_mmi49_fs *fs,
                     const char *approved_sd_path, size_t chunk_limit);
#endif
