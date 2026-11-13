#ifndef SPMP_GPSP_HEAP_BRIDGE_H
#define SPMP_GPSP_HEAP_BRIDGE_H
#include <stddef.h>
/* The module loader must hand over an already validated, exclusive RAM span.
   The core must be fully stopped and all allocations retired before rebind. */
int gpsp_heap_bind(void *ram, size_t bytes);
int gpsp_heap_unbind(void);
size_t gpsp_heap_remaining(void);
void *gpsp_heap_malloc(size_t bytes);
void *gpsp_heap_calloc(size_t count, size_t bytes);
void *gpsp_heap_realloc(void *ptr, size_t bytes);
void gpsp_heap_free(void *ptr);
int gpsp_heap_errors(void);
#endif
