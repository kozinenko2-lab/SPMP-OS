#include "spmp_ttf.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct input {uint8_t *data;uint32_t size,reads;};
static int read_at(void *v,uint32_t at,void *out,uint32_t n){struct input *in=v;assert(at<=in->size&&n<=in->size-at);memcpy(out,in->data+at,n);in->reads++;return 0;}
static unsigned ink(const struct spmp_ttf_glyph *g){unsigned n=0;assert(g);for(unsigned y=0;y<SPMP_TTF_HEIGHT;y++)for(unsigned x=0;x<16;x++)n+=!!(g->rows[y]&(1u<<x));return n;}
int main(int argc,char **argv){
 const char *path=argc>1?argv[1]:"assets/fonts/Roboto-Regular.ttf";FILE *file=fopen(path,"rb");assert(file);assert(!fseek(file,0,SEEK_END));long n=ftell(file);assert(n>0&&n<4000000);rewind(file);
 struct input in={malloc((size_t)n),(uint32_t)n,0};assert(in.data&&fread(in.data,1,in.size,file)==in.size);assert(!fclose(file));
 static union spmp_ttf_font font;struct spmp_ttf_io io={&in,in.size,read_at};assert(!spmp_ttf_open(&font,&io,13));
 const struct spmp_ttf_glyph *a=spmp_ttf_glyph(&font,'A');assert(ink(a)>10);unsigned reads=in.reads;assert(spmp_ttf_glyph(&font,'A')==a&&in.reads==reads);
 uint16_t arows[SPMP_TTF_HEIGHT];memcpy(arows,a->rows,sizeof arows);assert(!ink(spmp_ttf_glyph(&font,' ')));
 const struct spmp_ttf_glyph *z=spmp_ttf_glyph(&font,0x416);assert(ink(z)>10&&memcmp(arows,z->rows,sizeof arows));
 assert(ink(spmp_ttf_glyph(&font,0x401))>10); /* composite Cyrillic Yo */
 uint32_t digest=2166136261u;
 for(unsigned block=0;block<2;block++)for(uint32_t cp=block?0x400u:0x20u;cp<(block?0x460u:0x7fu);cp++){
  const struct spmp_ttf_glyph *g=spmp_ttf_glyph(&font,cp);assert(g);
  for(unsigned y=0;y<SPMP_TTF_HEIGHT;y++)digest=(digest^g->rows[y])*16777619u;
  digest=(digest^g->advance)*16777619u;
 }
 printf("Glyph digest: %08x\n",digest);
 assert(digest==0x449b251cu); /* Same samples as the reference point-winding renderer. */
 assert(spmp_ttf_cache_misses(&font)>100);
 assert(!spmp_ttf_open(&font,&io,16));assert(ink(spmp_ttf_glyph(&font,'A'))>10);
 struct spmp_ttf_io broken=io;broken.bytes=40;assert(spmp_ttf_open(&font,&broken,13));
 uint8_t keep=in.data[0];in.data[0]=0xff;assert(spmp_ttf_open(&font,&io,13));in.data[0]=keep;
 assert(spmp_ttf_open(&font,&io,0));
 uint32_t random=1;
 for(unsigned i=0;i<400;i++){
  random=random*1664525u+1013904223u;uint32_t at=random%in.size;uint8_t old=in.data[at];in.data[at]^=0xff;
  if(!spmp_ttf_open(&font,&io,13)){(void)spmp_ttf_glyph(&font,'A');(void)spmp_ttf_glyph(&font,0x401);}
  in.data[at]=old;
 }
 free(in.data);puts("PASS: direct streaming TTF, Latin/Cyrillic/composite glyphs, cache, sizes, truncated/invalid inputs; no pre-rasterized font");return 0;
}
