#ifndef SPMP_STAGE44_FLUSH_H
#define SPMP_STAGE44_FLUSH_H
#include "spmp_bootlog.h"
#include <stdint.h>
#include <stddef.h>
/* Backend is a verified OEM FS adapter, called ONLY from a dedicated
 * non-interrupt, post-FAT-mount logging task; no recursion from FAT IO.
 * close_checked must indicate success only if actual underlying close
 * succeeded; a no-op OEM fsync is insufficient for commit accounting.
 */
typedef struct {
    void *context;
    int (*ready)(void *context);
    int (*open_append)(void *context, const char *path);
    int (*write)(void *context,const char *data,size_t n);
    int (*close_checked)(void *context);
} spmp_stage44_fs;
typedef struct {
    uint32_t session, acknowledged_sequence, successful_closes;
    uint32_t io_failures;
    uint8_t active;
} spmp_stage44_flush_state;
/* Returns: 1 delivered to FS+closed; 0 not ready/no new records;
 * -1 error (factory boot MUST continue). Acknowledgement is NOT a
 * power-loss durability guarantee. Repeated sequences are deduplicable.
 */
int spmp_stage44_flush(const spmp_bootlog *log,
   spmp_stage44_flush_state *state, const spmp_stage44_fs *fs,
   uint32_t max_records);
#endif
