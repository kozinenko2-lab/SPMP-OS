#include "gpsp_heap_bridge.h"
#include "spmp_heap.h"
#include <stdint.h>
static spmp_heap heap;
static int bound;
static int errors;
int gpsp_heap_bind(void *ram,size_t n) {
    if(bound || !ram || spmp_heap_init(&heap,ram,n)) return -1;
    bound=1;errors=0;return 0;
}
/* Unbind is only legal after all buffers have been freed by libretro_deinit().
   A nonempty heap cannot be unbound, even though memory belongs to the loader. */
int gpsp_heap_unbind(void) {
    if(!bound || spmp_heap_check(&heap)) return -1;
    /* Compare total free block capacity, accounting for headers by attempting
       a large allocation would mutate state, so check with a dedicated API. */
    if(spmp_heap_busy_blocks(&heap)!=0) return -1;
    heap.base=0;heap.size=0;heap.first=0;bound=0;return 0;
}
size_t gpsp_heap_remaining(void){return bound?spmp_heap_free_bytes(&heap):0;}
void *gpsp_heap_malloc(size_t n){return bound?spmp_heap_alloc(&heap,n):0;}
void *gpsp_heap_calloc(size_t c,size_t n){return bound?spmp_heap_calloc(&heap,c,n):0;}
void *gpsp_heap_realloc(void *p,size_t n){return bound?spmp_heap_realloc(&heap,p,n):0;}
void gpsp_heap_free(void *p){if(bound && spmp_heap_free(&heap,p))errors++;}
int gpsp_heap_errors(void){return errors;}
