#ifndef SPMP_CEVA_SESSION_H
#define SPMP_CEVA_SESSION_H
#include <stddef.h>
#include <stdint.h>

#define SPMP_CEVA_REQUEST_CAPACITY 256u
#define SPMP_CEVA_REQUEST_HEADER 4u
#define SPMP_CEVA_REQUEST_PAYLOAD_MAX 252u
#define SPMP_CEVA_MAX_ALLOCATIONS 16u

typedef enum spmp_ceva_state {
 SPMP_CEVA_OFF=0,SPMP_CEVA_OS_EXT_LOADED,SPMP_CEVA_READY,
 SPMP_CEVA_OS_INT_LOADED,SPMP_CEVA_CODEC_BOOTED,SPMP_CEVA_STOPPING,
 SPMP_CEVA_FAILED
} spmp_ceva_state;
typedef enum spmp_ceva_event {
 SPMP_CEVA_LOAD_OS_EXT=1,SPMP_CEVA_SIGNAL_READY,SPMP_CEVA_LOAD_OS_INT,
 SPMP_CEVA_SIGNAL_CODEC_BOOTED,SPMP_CEVA_BEGIN_STOP,SPMP_CEVA_COMPLETE_STOP,
 SPMP_CEVA_FAIL
} spmp_ceva_event;
typedef struct spmp_ceva_allocation {uintptr_t pointer;uint32_t bytes;} spmp_ceva_allocation;
typedef struct spmp_ceva_session {
 spmp_ceva_state state;
 uint32_t allocation_limit,total_bytes,allocation_count;
 spmp_ceva_allocation allocations[SPMP_CEVA_MAX_ALLOCATIONS];
} spmp_ceva_session;
/* A nonzero result means the allocation is still owned and may be retried. */
typedef int (*spmp_ceva_release_fn)(void *context,uintptr_t pointer,uint32_t bytes);

void spmp_ceva_session_init(spmp_ceva_session *session,uint32_t allocation_limit);
int spmp_ceva_session_event(spmp_ceva_session *session,spmp_ceva_event event);
int spmp_ceva_request_validate(const uint8_t *packet,size_t available,uint8_t *opcode,
 uint8_t *no_reply,uint16_t *payload_bytes);
int spmp_ceva_track_allocation(spmp_ceva_session *session,uintptr_t pointer,uint32_t bytes);
int spmp_ceva_track_free(spmp_ceva_session *session,uintptr_t pointer,uint32_t *bytes);
/* STOPPING only; successful entries are forgotten, the first failed entry is retained. */
int spmp_ceva_cleanup_allocations(spmp_ceva_session *session,
 spmp_ceva_release_fn release,void *context,uint32_t *released_count);
#endif
