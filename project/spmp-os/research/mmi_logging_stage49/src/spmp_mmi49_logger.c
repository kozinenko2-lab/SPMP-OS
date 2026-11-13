#include "spmp_mmi49_logger.h"

void spmp_mmi49_init(spmp_mmi49_logger *l, int enable) {
    size_t i;
    if (!l) return;
    l->magic = SPMP_MMI49_MAGIC;
    l->used = l->acknowledged = l->dropped = 0;
    l->failed_flushes = l->successful_closes = 0;
    l->enabled = enable != 0;
    l->busy = 0;
    for (i = 0; i < SPMP_MMI49_CAPACITY; ++i) l->bytes[i] = 0;
}
void spmp_mmi49_capture_char(spmp_mmi49_logger *l, int ch) {
    if (!l || l->magic != SPMP_MMI49_MAGIC || !l->enabled) return;
    if (l->used == SPMP_MMI49_CAPACITY) { if (l->dropped != UINT32_MAX) l->dropped++; return; }
    if (ch < 0 || ch > 255) ch = '?';
    l->bytes[l->used++] = (char)ch;
}
void spmp_mmi49_capture_text(spmp_mmi49_logger *l, const char *s) {
    unsigned i = 0;
    if (!s) return;
    while (s[i] && i < 128u) spmp_mmi49_capture_char(l, (unsigned char)s[i++]);
}
static void hex32(spmp_mmi49_logger *l, uint32_t val) {
    const char h[]="0123456789ABCDEF";
    unsigned i;
    for(i=0;i<8;i++) spmp_mmi49_capture_char(l, h[(val >> (28u - 4u*i))&15u]);
}
void spmp_mmi49_event(spmp_mmi49_logger *l, uint32_t tick, const char *tag, uint32_t value) {
    spmp_mmi49_capture_text(l,"[MMI t="); hex32(l,tick);
    spmp_mmi49_capture_text(l,"] "); spmp_mmi49_capture_text(l,tag);
    spmp_mmi49_capture_text(l," 0x"); hex32(l,value);
    spmp_mmi49_capture_char(l,'\n');
}
void spmp_mmi49_forward(spmp_mmi49_logger *l, void (*original)(void *, int), void *ctx, int ch) {
    /* Integration must verify original points at genuine unmodified putc. */
    if (original) original(ctx,ch);
    spmp_mmi49_capture_char(l,ch);
}
/* Require explicit /fat20aN/hdaN/ prefix, never allow arbitrary relative
 * path (which might refer to NAND/ROMFS), no '..', backslash, or controls. */
static int approved(const char *path) {
    static const char prefix[]="/fat20a";
    size_t i, j=0;
    if(!path) return 0;
    for(i=0; prefix[i]; i++) if(path[i]!=prefix[i])return 0;
    if(path[i]<'0'||path[i]>'9')return 0;
    i++;
    if(path[i++]!='/')return 0;
    if(path[i++]!='h'||path[i++]!='d'||path[i++]!='a')return 0;
    if(path[i]<'0'||path[i]>'9')return 0;
    i++;
    if(path[i++]!='/')return 0;
    if(!path[i])return 0;
    for(;path[i] && j<64u;i++,j++) {
        unsigned char c=(unsigned char)path[i];
        if(c<' ' || c>=127 || c=='\\') return 0;
        if(c=='.' && path[i+1]=='.')return 0;
    }
    return j>0 && j<64u && !path[i];
}
int spmp_mmi49_flush(spmp_mmi49_logger *l, const spmp_mmi49_fs *fs,
                     const char *approved_sd_path, size_t chunk_limit) {
    int wrote, closed;
    size_t n;
    if(!l || l->magic!=SPMP_MMI49_MAGIC || !fs || !fs->sd_ready ||
       !fs->open_append || !fs->write || !fs->close_checked || l->busy ||
       l->acknowledged>l->used || l->used>SPMP_MMI49_CAPACITY ||
       !approved(approved_sd_path))return -1;
    if(!l->enabled || l->acknowledged==l->used) return 0;
    if(fs->sd_ready(fs->ctx)!=1) return 0;
    if(chunk_limit==0 || chunk_limit>512u) chunk_limit=512u;
    n=(size_t)(l->used-l->acknowledged);
    if(n>chunk_limit)n=chunk_limit;
    l->busy=1;
    if(fs->open_append(fs->ctx,approved_sd_path)!=1){
        l->failed_flushes++; l->busy=0; return -1;
    }
    wrote=fs->write(fs->ctx,l->bytes+l->acknowledged,n);
    closed=fs->close_checked(fs->ctx);
    if(wrote!=(int)n || closed!=1){
        l->failed_flushes++; l->busy=0; return -1;
    }
    l->acknowledged+=(uint32_t)n;
    l->successful_closes++;
    l->busy=0;
    return 1;
}
