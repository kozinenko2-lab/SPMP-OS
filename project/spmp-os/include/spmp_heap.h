#ifndef SPMP_HEAP_H
#define SPMP_HEAP_H
#include <stddef.h>
/* Single-threaded bounded allocator. Heap never grows outside caller memory. */
typedef struct spmp_heap { unsigned char *base; size_t size; union block *first; } spmp_heap;
int spmp_heap_init(spmp_heap *heap,void *memory,size_t bytes);
int spmp_heap_check(const spmp_heap *heap);
void *spmp_heap_alloc(spmp_heap *heap,size_t bytes);
void *spmp_heap_calloc(spmp_heap *heap,size_t count,size_t bytes);
void *spmp_heap_realloc(spmp_heap *heap,void *ptr,size_t bytes);
int spmp_heap_free(spmp_heap *heap,void *ptr);
size_t spmp_heap_free_bytes(const spmp_heap *heap);
size_t spmp_heap_busy_blocks(const spmp_heap *heap);
#endif
