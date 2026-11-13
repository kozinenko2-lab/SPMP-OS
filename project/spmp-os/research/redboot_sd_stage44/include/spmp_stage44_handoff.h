#ifndef SPMP_STAGE44_HANDOFF_H
#define SPMP_STAGE44_HANDOFF_H
#include <stddef.h>
#include <stdint.h>
#include "spmp_bootlog.h"

/* Copy-only RAM handoff; no fixed address, no direct hardware access.
 * A board-specific, independently validated memory map must authorize
 * the exact address, size, lifetime and cache policy on both boot stages.
 * Never place this in ROM, NAND, MMIO, or unverified DRAM.
 * Not safe to use from interrupt context or while the journal is modified.
 */
#define SPMP_HANDOFF_MAGIC 0x34344c53u /* SL44 */
#define SPMP_HANDOFF_VERSION 1u
#define SPMP_HANDOFF_READY 0x434f4d4du /* COMM, written last */
typedef struct {
    uint32_t magic, version, header_bytes, payload_bytes;
    uint32_t session, generation, payload_crc32, commit;
} spmp_stage44_handoff_header;
#define SPMP_HANDOFF_SIZE (sizeof(spmp_stage44_handoff_header)+sizeof(spmp_bootlog))
/* Caller must pass authorized_region=1 only after a board-specific
 * verified memory reservation. Returns 1 success, 0 refusal. */
int spmp_stage44_handoff_pack(void *region, size_t region_bytes,
    const spmp_bootlog *log, uint32_t generation, int authorized_region);
int spmp_stage44_handoff_unpack(const void *region, size_t region_bytes,
    spmp_bootlog *destination, uint32_t expected_session,
    uint32_t expected_generation, int authorized_region);
#endif
