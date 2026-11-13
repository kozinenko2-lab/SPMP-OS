#include "spmp_heap.h"
#include <stdint.h>
#include <stddef.h>

/* A freestanding, bounded, single-threaded allocator for one resident core.
 * Not a replacement for libc malloc until every core allocation is routed
 * through these explicit functions. The caller owns the RAM region. */
#define H_MAGIC 0x53484d50u
#define H_FREE  0x46524545u
#define H_BUSY  0x42555359u
#define H_ALIGN ((size_t)_Alignof(max_align_t))

typedef union block {
    struct { union block *next; size_t capacity; uint32_t magic; uint32_t state; } m;
    max_align_t alignment;
} block;

static int align_up(size_t n, size_t *out) {
    if (n > SIZE_MAX - (H_ALIGN - 1u)) return -1;
    *out = (n + H_ALIGN - 1u) & ~(H_ALIGN - 1u);
    return 0;
}
static unsigned char *payload(block *b) { return (unsigned char *)b + sizeof(block); }
static int valid(const spmp_heap *h) {
    if (!h || !h->first || !h->base || !h->size) return 0;
    uintptr_t lo=(uintptr_t)h->base;
    if (h->size > UINTPTR_MAX - lo) return 0;
    uintptr_t hi=lo+h->size;
    uintptr_t pos=lo;
    const block *b=h->first;
    size_t max_steps=h->size/sizeof(block)+1;
    for (size_t i=0;b && i<max_steps;i++) {
        if ((uintptr_t)b!=pos || (uintptr_t)b>hi || hi-(uintptr_t)b<sizeof(block)) return 0;
        if (b->m.magic!=H_MAGIC || (b->m.state!=H_FREE && b->m.state!=H_BUSY)) return 0;
        uintptr_t start=(uintptr_t)b+sizeof(block);
        if (b->m.capacity>hi-start) return 0;
        pos=start+b->m.capacity;
        if (!b->m.next) return pos==hi;
        b=b->m.next;
    }
    return 0;
}
int spmp_heap_init(spmp_heap *h,void *memory,size_t size) {
    if (!h || !memory) return -1;
    h->base=0;h->size=0;h->first=0;
    uintptr_t addr=(uintptr_t)memory;
    uintptr_t padding=(H_ALIGN-(addr&(H_ALIGN-1u)))&(H_ALIGN-1u);
    if (padding>size || size-padding<=sizeof(block)+H_ALIGN) return -1;
    addr+=padding;size-=padding;
    size &= ~(H_ALIGN-1u);
    h->base=(unsigned char *)addr;h->size=size;
    block *b=(block *)h->base;
    b->m.next=0;b->m.capacity=size-sizeof(block);b->m.magic=H_MAGIC;b->m.state=H_FREE;
    h->first=b;
    return 0;
}
int spmp_heap_check(const spmp_heap *h) { return valid(h) ? 0 : -1; }
void *spmp_heap_alloc(spmp_heap *h,size_t n) {
    size_t want;
    if (!n || align_up(n,&want) || !valid(h)) return 0;
    for (block *b=h->first;b;b=b->m.next) {
        if (b->m.state!=H_FREE || b->m.capacity<want) continue;
        size_t remainder=b->m.capacity-want;
        if (remainder>=sizeof(block)+H_ALIGN) {
            block *next=(block *)(payload(b)+want);
            next->m.next=b->m.next;
            next->m.capacity=remainder-sizeof(block);
            next->m.magic=H_MAGIC;next->m.state=H_FREE;
            b->m.next=next;b->m.capacity=want;
        }
        b->m.state=H_BUSY;
        return payload(b);
    }
    return 0;
}
static block *find_block(spmp_heap *h,const void *p) {
    if (!p || !valid(h)) return 0;
    for (block *b=h->first;b;b=b->m.next)
        if (payload(b)==(const unsigned char *)p) return b;
    return 0;
}
static void coalesce(spmp_heap *h) {
    for (block *b=h->first;b && b->m.next;) {
        block *n=b->m.next;
        if (b->m.state==H_FREE && n->m.state==H_FREE) {
            b->m.capacity+=sizeof(block)+n->m.capacity;
            b->m.next=n->m.next;
        } else b=n;
    }
}
int spmp_heap_free(spmp_heap *h,void *p) {
    if (!p) return valid(h)?0:-1;
    block *b=find_block(h,p);
    if (!b || b->m.state!=H_BUSY) return -1;
    b->m.state=H_FREE;coalesce(h);return 0;
}
void *spmp_heap_calloc(spmp_heap *h,size_t count,size_t size) {
    if (size && count>SIZE_MAX/size) return 0;
    size_t n=count*size;
    unsigned char *p=spmp_heap_alloc(h,n);
    if (p) for(size_t i=0;i<n;i++)p[i]=0;
    return p;
}
void *spmp_heap_realloc(spmp_heap *h,void *old,size_t size) {
    if (!old) return spmp_heap_alloc(h,size);
    if (!size) {spmp_heap_free(h,old);return 0;}
    block *b=find_block(h,old);
    if (!b || b->m.state!=H_BUSY) return 0;
    if (size<=b->m.capacity) return old;
    size_t old_size=b->m.capacity;
    void *newp=spmp_heap_alloc(h,size);
    if (!newp) return 0;
    unsigned char *dst=newp,*src=old;
    for(size_t i=0;i<old_size;i++)dst[i]=src[i];
    spmp_heap_free(h,old);
    return newp;
}
size_t spmp_heap_free_bytes(const spmp_heap *h) {
    if (!valid(h)) return 0;
    size_t n=0;
    for(const block *b=h->first;b;b=b->m.next)if(b->m.state==H_FREE)n+=b->m.capacity;
    return n;
}

size_t spmp_heap_busy_blocks(const spmp_heap *h) {
    if (!valid(h)) return SIZE_MAX;
    size_t n=0;
    for(const block *b=h->first;b;b=b->m.next) if(b->m.state==H_BUSY) n++;
    return n;
}
