/* ARMv5-friendly, no unaligned 32-bit loads and no NEON assumptions. */
#include "spmp_psx_opt.h"
/* libc functions supplied by the SPMP-OS core on target. */
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern void *memset(void *, int, size_t);
static spmp_psx_icache_fn cache_cb;
static void *cache_user;
int spmp_psx_bind_icache(spmp_psx_icache_fn cb, void *user) {
    if (!cb) return 0;
    cache_cb = cb; cache_user = user;
    return 1;
}
void spmp_psx_unbind_icache(void) { cache_cb=0; cache_user=0; }
int spmp_psx_icache_ready(void) { return cache_cb != 0; }
void spmp_psx_icache_sync(uint32_t begin, uint32_t end, uint32_t flags) {
    (void)flags;
    if (!cache_cb || end < begin) return;
    cache_cb((uintptr_t)begin, (uintptr_t)end, cache_user);
}
static int rect_ok(unsigned width, unsigned height, int x, int y, unsigned w, unsigned h) {
    if (!width || !height || x < 0 || y < 0) return 0;
    if ((unsigned)x >= width || (unsigned)y >= height) return 0;
    return w <= width-(unsigned)x && h <= height-(unsigned)y;
}
int spmp_psx_gpu_move_16(uint16_t *vram, unsigned width, unsigned height,
                         int sx, int sy, int dx, int dy, unsigned w, unsigned h) {
    unsigned row;
    if (!vram || !rect_ok(width,height,sx,sy,w,h) ||
        !rect_ok(width,height,dx,dy,w,h)) return 0;
    if (!w || !h || (sx==dx && sy==dy)) return 1;
    if (dy>sy && (unsigned)(dy-sy)<h) {
        /* Bottom-to-top so overlapping VRAM rectangles work like a snapshot. */
        for (row=h;row;--row)
            memmove(vram+((unsigned)dy+row-1)*width+(unsigned)dx,
                    vram+((unsigned)sy+row-1)*width+(unsigned)sx, w*sizeof(uint16_t));
    } else {
        for (row=0;row<h;++row)
            memmove(vram+((unsigned)dy+row)*width+(unsigned)dx,
                    vram+((unsigned)sy+row)*width+(unsigned)sx, w*sizeof(uint16_t));
    }
    return 1;
}
int spmp_psx_gpu_clear_16(uint16_t *vram, unsigned width, unsigned height,
                          int x, int y, unsigned w, unsigned h, uint16_t color) {
    unsigned row, col;
    if (!vram || !rect_ok(width,height,x,y,w,h)) return 0;
    if (!w || !h) return 1;
    for (row=0;row<h;row++) {
        uint16_t *dest=vram+((unsigned)y+row)*width+(unsigned)x;
        if (color==0) { memset(dest,0,(size_t)w*2); continue; }
        /* memcpy avoids strict-aliasing and alignment undefined behavior. */
        uint32_t pair=(uint32_t)color|((uint32_t)color<<16);
        for (col=0;col+1<w;col+=2) memcpy(dest+col,&pair,sizeof(pair));
        if (col<w) dest[col]=color;
    }
    return 1;
}

int spmp_psx_gpu_dma_walk(const uint32_t *ram, unsigned bytes,
                          uint32_t next, unsigned budget,
                          spmp_psx_gpu_cmd_sink sink, void *user) {
    unsigned nodes=0;
    if (!ram || !sink || bytes != 0x200000u || !budget) return -1;
    next &= 0x00ffffffu;
    while (next != 0x00ffffffu) {
        uint32_t offset=next & 0x001ffffcu;
        uint32_t header=ram[offset>>2];
        unsigned count=header>>24;
        unsigned start=((offset>>2)+1u)&0x7ffffu;
        if (++nodes>budget) return -1;
        if (start+count <= 0x80000u) {
            for (unsigned i=0;i<count;i++) sink(ram[start+i],user);
        } else {
            for (unsigned i=0;i<count;i++) sink(ram[(start+i)&0x7ffffu],user);
        }
        next=header & 0x00ffffffu;
    }
    return (int)nodes;
}
