#ifndef SPMP_MM_COMMON_H
#define SPMP_MM_COMMON_H
#include "spmp_mm.h"
static inline uint16_t rd16(const uint8_t *p){return (uint16_t)(p[0]|(uint16_t)p[1]<<8);}
static inline uint32_t rd32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static inline uint32_t be32(const uint8_t *p){return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];}
static inline uint16_t be16(const uint8_t *p){return (uint16_t)((uint16_t)p[0]<<8|p[1]);}
static inline int same4(const uint8_t *p,const char *s){return p[0]==(uint8_t)s[0]&&p[1]==(uint8_t)s[1]&&p[2]==(uint8_t)s[2]&&p[3]==(uint8_t)s[3];}
static inline int mm_read(const mm_reader *r,uint64_t off,void *dst,size_t size){
 if(!r||!r->read_at||off>r->size||size>r->size-off)return -1;
 return r->read_at(r->ctx,off,dst,size);
}
static inline void mm_copy4(char *dst,const uint8_t *src){unsigned i;for(i=0;i<4;i++)dst[i]=(char)src[i];dst[4]=0;}
static inline void mm_zero(void *dst,size_t size){size_t i;uint8_t *p=(uint8_t*)dst;for(i=0;i<size;i++)p[i]=0;}
static inline uint16_t mm_rgb565(unsigned r,unsigned g,unsigned b){return (uint16_t)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));}
#endif
