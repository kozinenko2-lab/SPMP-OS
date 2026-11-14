#ifndef SPMP_TTF_H
#define SPMP_TTF_H
#include <stddef.h>
#include <stdint.h>
#define SPMP_TTF_CONTEXT_BYTES 32768u
#define SPMP_TTF_HEIGHT 24u
/* Streaming TrueType glyf renderer, integer arithmetic, no heap or hint VM. */
struct spmp_ttf_io {void *user;uint32_t bytes;int (*read_at)(void *,uint32_t,void *,uint32_t);};
union spmp_ttf_font {uint64_t align;uint8_t opaque[SPMP_TTF_CONTEXT_BYTES];};
struct spmp_ttf_glyph {uint16_t rows[SPMP_TTF_HEIGHT];uint8_t advance,width,height;};
int spmp_ttf_open(union spmp_ttf_font *,const struct spmp_ttf_io *,unsigned pixel_size);
const struct spmp_ttf_glyph *spmp_ttf_glyph(union spmp_ttf_font *,uint32_t codepoint);
unsigned spmp_ttf_cache_misses(const union spmp_ttf_font *);
#endif
