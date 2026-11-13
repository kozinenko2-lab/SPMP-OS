#include "spmp_psx_cache_guard.h"
#include "spmp_psx_opt.h"
#define PSX_MAX_JIT_WINDOW (8u * 1024u * 1024u)
static spmp_psx_cache_guard *active_guard;
/* ARM926EJ-S default L1 cache line size is 32 bytes. Verify actual SoC config. */
static void guarded_sync(uintptr_t begin, uintptr_t end, void *ctx) {
    spmp_psx_cache_guard *g = (spmp_psx_cache_guard *)ctx;
    uintptr_t line, stop;
    if (!g || g->fault) return;
    if (begin < g->first || end > g->limit || begin >= end ||
        g->line_bytes != 32u || end - begin > PSX_MAX_JIT_WINDOW) {
        g->fault=1;g->rejected++;spmp_psx_unbind_icache();return;
    }
    line=begin & ~(uintptr_t)31u;
    /* No addition past UINTPTR_MAX; [first,limit) was previously validated. */
    stop=(end + 31u) & ~(uintptr_t)31u;
    for (uintptr_t a=line;a<stop;a+=32u) g->ops.dcache_clean_line(a);
    g->ops.write_buffer_drain();
    for (uintptr_t a=line;a<stop;a+=32u) g->ops.icache_invalidate_line(a);
    g->ops.prefetch_flush();
    g->sync_calls++;
}
int spmp_psx_cache_guard_attach(spmp_psx_cache_guard *g,
    uintptr_t first,size_t bytes,unsigned verified_privileged,
    const spmp_psx_cache_ops *ops) {
    if (!g || spmp_psx_icache_ready() || !verified_privileged || !ops || !ops->dcache_clean_line ||
        !ops->write_buffer_drain || !ops->icache_invalidate_line ||
        !ops->prefetch_flush || !first || (first & 31u) ||
        !bytes || bytes > PSX_MAX_JIT_WINDOW || (bytes & 31u) ||
        first > UINTPTR_MAX-bytes-31u) return 0;
    /* Do not overwrite the previous hook silently; use a safe shutdown first. */
    g->first=first;g->limit=first+bytes;g->line_bytes=32u;
    g->sync_calls=0;g->rejected=0;g->fault=0;g->ops=*ops;
    if (!spmp_psx_bind_icache(guarded_sync,g)) {g->fault=1;return 0;}
    active_guard=g;
    return 1;
}
void spmp_psx_cache_guard_detach(spmp_psx_cache_guard *g) {
    if (active_guard==g) {spmp_psx_unbind_icache();active_guard=0;}
    if (g) {g->fault=1;g->first=0;g->limit=0;}
}
int spmp_psx_cache_guard_faulted(const spmp_psx_cache_guard *g) {
    return !g || g->fault;
}

int spmp_psx_cache_guard_verified_range(uintptr_t start,size_t size) {
    return active_guard && !active_guard->fault && spmp_psx_icache_ready() &&
           size && size <= PSX_MAX_JIT_WINDOW && start==active_guard->first &&
           size==active_guard->limit-start;
}
void spmp_psx_cache_guard_reset_binding(void) {
    spmp_psx_unbind_icache();
    active_guard=0;
}
