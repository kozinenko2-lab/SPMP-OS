#include "spmp_ceva_session.h"

void spmp_ceva_session_init(spmp_ceva_session *s,uint32_t limit){
 uint32_t i;
 if(!s)return;
 s->state=SPMP_CEVA_OFF;s->allocation_limit=limit;s->total_bytes=0;s->allocation_count=0;
 for(i=0;i<SPMP_CEVA_MAX_ALLOCATIONS;i++){
  s->allocations[i].pointer=0;s->allocations[i].bytes=0;
 }
}
int spmp_ceva_session_event(spmp_ceva_session *s,spmp_ceva_event e){
 spmp_ceva_state next;if(!s)return -1;
 if(e==SPMP_CEVA_FAIL){if(s->state==SPMP_CEVA_OFF)return -1;s->state=SPMP_CEVA_FAILED;return 0;}
 next=s->state;
 switch(s->state){
  case SPMP_CEVA_OFF:if(e==SPMP_CEVA_LOAD_OS_EXT)next=SPMP_CEVA_OS_EXT_LOADED;else return -1;break;
  case SPMP_CEVA_OS_EXT_LOADED:if(e==SPMP_CEVA_SIGNAL_READY)next=SPMP_CEVA_READY;else if(e==SPMP_CEVA_BEGIN_STOP)next=SPMP_CEVA_STOPPING;else return -1;break;
  case SPMP_CEVA_READY:if(e==SPMP_CEVA_LOAD_OS_INT)next=SPMP_CEVA_OS_INT_LOADED;else if(e==SPMP_CEVA_BEGIN_STOP)next=SPMP_CEVA_STOPPING;else return -1;break;
  case SPMP_CEVA_OS_INT_LOADED:if(e==SPMP_CEVA_SIGNAL_CODEC_BOOTED)next=SPMP_CEVA_CODEC_BOOTED;else if(e==SPMP_CEVA_BEGIN_STOP)next=SPMP_CEVA_STOPPING;else return -1;break;
  case SPMP_CEVA_CODEC_BOOTED:if(e==SPMP_CEVA_BEGIN_STOP)next=SPMP_CEVA_STOPPING;else return -1;break;
  case SPMP_CEVA_FAILED:if(e==SPMP_CEVA_BEGIN_STOP)next=SPMP_CEVA_STOPPING;else return -1;break;
  case SPMP_CEVA_STOPPING:
   if(e!=SPMP_CEVA_COMPLETE_STOP||s->allocation_count!=0)return -1;
   next=SPMP_CEVA_OFF;break;
  default:return -1;
 }
 s->state=next;return 0;
}
int spmp_ceva_request_validate(const uint8_t *p,size_t n,uint8_t *opcode,
 uint8_t *no_reply,uint16_t *payload_bytes){
 uint16_t payload;size_t total;if(!p||n<SPMP_CEVA_REQUEST_HEADER)return -1;
 payload=(uint16_t)((uint16_t)p[2]|((uint16_t)p[3]<<8));total=4u+(size_t)payload;
 if(payload>SPMP_CEVA_REQUEST_PAYLOAD_MAX||total>n||p[0]<0x40u||p[0]>0x46u)return -1;
 if(opcode)*opcode=p[0];
 if(no_reply)*no_reply=(uint8_t)(p[1]!=0);
 if(payload_bytes)*payload_bytes=payload;
 return 0;
}
int spmp_ceva_track_allocation(spmp_ceva_session *s,uintptr_t pointer,uint32_t bytes){
 uint32_t i;if(!s||!pointer||(pointer&3u)!=0||!bytes||
  (s->state!=SPMP_CEVA_READY&&s->state!=SPMP_CEVA_OS_INT_LOADED&&
   s->state!=SPMP_CEVA_CODEC_BOOTED)||s->allocation_count>=SPMP_CEVA_MAX_ALLOCATIONS)return -1;
 if(UINT32_MAX-s->total_bytes<bytes||s->total_bytes+bytes>s->allocation_limit)return -1;
 for(i=0;i<SPMP_CEVA_MAX_ALLOCATIONS;i++){
  if(s->allocations[i].pointer==pointer)return -1;
  if(s->allocations[i].pointer==0){s->allocations[i].pointer=pointer;s->allocations[i].bytes=bytes;
   s->allocation_count++;s->total_bytes+=bytes;return 0;}
 }return -1;
}
int spmp_ceva_track_free(spmp_ceva_session *s,uintptr_t pointer,uint32_t *bytes){
 uint32_t i,released;if(!s||!pointer)return -1;
 for(i=0;i<SPMP_CEVA_MAX_ALLOCATIONS;i++)if(s->allocations[i].pointer==pointer){
  released=s->allocations[i].bytes;s->allocations[i].pointer=0;s->allocations[i].bytes=0;
  s->allocation_count--;s->total_bytes-=released;if(bytes)*bytes=released;return 0;
 }return -1;
}
