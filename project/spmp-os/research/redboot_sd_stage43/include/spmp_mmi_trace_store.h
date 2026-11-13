#ifndef SPMP_MMI_TRACE_STORE_H
#define SPMP_MMI_TRACE_STORE_H
#include "spmp_bootlog.h"
/* MMI side: only call AFTER genuine FAT mounting, using a verified OEM
 * filesystem wrapper. Does not write SD sectors directly or install hooks. */
typedef struct {
    void *context;
    int (*fs_ready_writable)(void *context); /* 1 only on mounted SD FAT */
    int (*open_append)(void *context,const char *path); /* 1 success */
    int (*write)(void *context,const char *data,size_t n); /* bytes written */
    int (*sync)(void *context); /* 1 success */
    void (*close)(void *context);
} spmp_mmi_filesystem;
typedef struct {
    uint32_t session;
    uint32_t committed_sequence;
    uint32_t successful_flushes;
    uint32_t write_failures;
    uint32_t pending_loss_count;
} spmp_mmi_trace_state;
/* After each successful sync, progress cursor is committed. The caller must
 * persist the cursor itself only if boot-session persistence is desired.
 * Return: 1 flushed, 0 skipped (not ready / no new records), -1 I/O failure.
 */
int spmp_mmi_trace_flush(const spmp_bootlog *journal,
                         spmp_mmi_trace_state *state,
                         const spmp_mmi_filesystem *fs);
#endif
