#ifndef SPMP_MEMORY_H
#define SPMP_MEMORY_H
#include <stddef.h>
#include <stdint.h>
/* All sizes and addresses are supplied by the platform; NO assumed Defender
 * physical DRAM boundaries. Sections stay disjoint, guarded and aligned. */
#define SPMP_MEMORY_ALIGNMENT 64u
#define SPMP_MEMORY_GUARD_BYTES 32u

typedef struct {void *ptr;size_t size;} spmp_region;
typedef struct {
 size_t system, core, game, video, audio;
} spmp_partition_request;
typedef struct {
 spmp_region system,core,game,video,audio;
 size_t used,unused;
 uint8_t *start;
} spmp_partition;
int spmp_memory_partition(void *ram,size_t capacity,
 const spmp_partition_request *request,spmp_partition *result);
int spmp_memory_check_guards(const spmp_partition *partition);

typedef struct {
 uint8_t *memory;
 size_t capacity,used,peak;
} spmp_arena;
void spmp_arena_init(spmp_arena *a,void *mem,size_t capacity);
void *spmp_arena_alloc(spmp_arena *a,size_t n,size_t align);
void spmp_arena_reset(spmp_arena *a);
#endif
