/* SPDX-License-Identifier: MIT
 * Small bounded TrueType glyf renderer. Quadratic outlines, XY composites,
 * cmap4/12, monochrome scan conversion. No CFF/OTF, hint execution or shaping.
 */
#include "spmp_ttf.h"
#define POINTS 512u
#define CONTOURS 128u
#define EDGES 1024u
#define CACHE 96u
struct point {int32_t x,y;uint8_t on;};
struct edge {int32_t x0,y0,x1,y1;};
struct table {uint32_t at,bytes;};
struct transform {int32_t a,b,c,d,x,y;};
struct cached {uint32_t cp,valid;struct spmp_ttf_glyph glyph;};
struct font {
 struct spmp_ttf_io io;
 struct table head,hhea,hmtx,maxp,loca,glyf,cmap;
 uint32_t cmap_at,cmap_bytes,cmap_format,units,glyphs,metrics,loc_format,pixels,baseline;
 uint32_t cache_cursor,misses,edge_count,valid,shapes;
 struct point points[POINTS];uint16_t ends[CONTOURS];
 struct edge edges[EDGES];struct cached cache[CACHE];
};
_Static_assert(sizeof(struct font)<=SPMP_TTF_CONTEXT_BYTES,"TrueType fixed-memory context");
static int read(const struct font *f,uint32_t at,void *out,uint32_t n){
 if(at>f->io.bytes||n>f->io.bytes-at)return -1;
 return f->io.read_at(f->io.user,at,out,n);
}
static int word(const struct font *f,uint32_t at,uint32_t *out){uint8_t b[4];if(read(f,at,b,4))return -1;*out=((uint32_t)b[0]<<24)|((uint32_t)b[1]<<16)|((uint32_t)b[2]<<8)|b[3];return 0;}
static int half(const struct font *f,uint32_t at,uint32_t *out){uint8_t b[2];if(read(f,at,b,2))return -1;*out=((uint32_t)b[0]<<8)|b[1];return 0;}
static int16_t signed16(uint32_t n){return (int16_t)(uint16_t)n;}
static int table_half(const struct font *f,struct table t,uint32_t at,uint32_t *out){return at>t.bytes||t.bytes-at<2?-1:half(f,t.at+at,out);}
static int next_half(const struct font *f,uint32_t *at,uint32_t end,uint32_t *out){if(*at>end||end-*at<2||half(f,*at,out))return -1;*at+=2;return 0;}
static int next_byte(const struct font *f,uint32_t *at,uint32_t end,uint32_t *out){uint8_t b;if(*at>=end||read(f,*at,&b,1))return -1;(*at)++;*out=b;return 0;}
int spmp_ttf_open(union spmp_ttf_font *memory,const struct spmp_ttf_io *io,unsigned px){
 if(!memory||!io||!io->read_at||io->bytes<12||io->bytes>4u*1024u*1024u||px<6||px>20)return -1;
 struct font *f=(struct font*)memory->opaque;uint8_t *p=(uint8_t*)f;for(size_t i=0;i<sizeof *f;i++)p[i]=0;
 f->io.user=io->user;f->io.bytes=io->bytes;f->io.read_at=io->read_at;uint32_t signature,count;
 if(word(f,0,&signature)||signature!=0x10000||half(f,4,&count)||!count||count>64||12+count*16>io->bytes)return -1;
 for(uint32_t i=0;i<count;i++){
  uint32_t tag,at,n;if(word(f,12+i*16,&tag)||word(f,20+i*16,&at)||word(f,24+i*16,&n)||at>io->bytes||n>io->bytes-at)return -1;
  struct table *t=tag==0x68656164?&f->head:tag==0x68686561?&f->hhea:tag==0x686d7478?&f->hmtx:tag==0x6d617870?&f->maxp:tag==0x6c6f6361?&f->loca:tag==0x676c7966?&f->glyf:tag==0x636d6170?&f->cmap:0;
  if(t){if(t->bytes)return -1;t->at=at;t->bytes=n;}
 }
 uint32_t format,asc;
 if(table_half(f,f->head,18,&f->units)||f->units<16||f->units>16384||table_half(f,f->head,50,&f->loc_format)||f->loc_format>1||
    table_half(f,f->hhea,4,&asc)||signed16(asc)<=0||table_half(f,f->hhea,34,&f->metrics)||!f->metrics||
    table_half(f,f->maxp,4,&f->glyphs)||!f->glyphs||f->metrics>f->glyphs||f->hmtx.bytes<f->metrics*4u||
    f->loca.bytes<(f->glyphs+1u)*(f->loc_format?4u:2u)||!f->glyf.bytes||table_half(f,f->cmap,2,&count)||count>64||f->cmap.bytes<4+count*8)return -1;
 f->pixels=px;f->baseline=((uint32_t)signed16(asc)*px+f->units/2)/f->units;
 if(f->baseline>=SPMP_TTF_HEIGHT)return -1;
 for(uint32_t i=0;i<count;i++){
  uint32_t platform,encoding,off;
  if(half(f,f->cmap.at+4+i*8,&platform)||half(f,f->cmap.at+6+i*8,&encoding)||word(f,f->cmap.at+8+i*8,&off)||off>f->cmap.bytes||f->cmap.bytes-off<4)return -1;
  if(half(f,f->cmap.at+off,&format))return -1;
  if(!((platform==0)||(platform==3&&(encoding==1||encoding==10))))continue;
  uint32_t n;
  if(format==4){if(half(f,f->cmap.at+off+2,&n)||n<16)continue;}
  else if(format==12){if(f->cmap.bytes-off<16||word(f,f->cmap.at+off+4,&n)||n<16)continue;}
  else continue;
  if(n>f->cmap.bytes-off)return -1;
  if(!f->cmap_format||format==12){f->cmap_at=f->cmap.at+off;f->cmap_bytes=n;f->cmap_format=format;}
 }
 if(!f->cmap_format)return -1;
 f->valid=1;return 0;
}
static int glyph_index(const struct font *f,uint32_t cp,uint32_t *out){
 if(f->cmap_format==12){
  uint32_t n;if(word(f,f->cmap_at+12,&n)||n>4096||n>(f->cmap_bytes-16)/12)return -1;
  for(uint32_t i=0;i<n;i++){uint32_t a,b,g,at=f->cmap_at+16+i*12;if(word(f,at,&a)||word(f,at+4,&b)||word(f,at+8,&g)||b<a)return -1;if(cp>=a&&cp<=b){if(cp-a>0xffffu||g>0xffffu-(cp-a))return -1;*out=g+cp-a;return *out<f->glyphs?0:-1;}}
 }else if(cp<=0xffffu){
  uint32_t n;if(half(f,f->cmap_at+6,&n)||!n||(n&1u))return -1;n/=2;
  if(n>4096||n>(f->cmap_bytes-16)/8)return -1;
  for(uint32_t i=0;i<n;i++){
   uint32_t end,start,delta,range;uint32_t ra=16+6*n+i*2;
   if(half(f,f->cmap_at+14+i*2,&end)||half(f,f->cmap_at+16+2*n+i*2,&start)||half(f,f->cmap_at+16+4*n+i*2,&delta)||half(f,f->cmap_at+ra,&range))return -1;
   if(cp<start||cp>end)continue;
   uint32_t g;
   if(!range)g=(cp+delta)&0xffffu;
   else{uint32_t off=ra+range+(cp-start)*2;if(off>f->cmap_bytes||f->cmap_bytes-off<2||half(f,f->cmap_at+off,&g))return -1;if(g)g=(g+delta)&0xffffu;}
   *out=g;return g<f->glyphs?0:-1;
  }
 }
 *out=0;return 0;
}
static int location(const struct font *f,uint32_t glyph,uint32_t *at,uint32_t *end){
 if(glyph>=f->glyphs)return -1;
 uint32_t a,b;
 if(f->loc_format){if(word(f,f->loca.at+glyph*4,&a)||word(f,f->loca.at+(glyph+1)*4,&b))return -1;}
 else{if(half(f,f->loca.at+glyph*2,&a)||half(f,f->loca.at+(glyph+1)*2,&b))return -1;a*=2;b*=2;}
 if(b<a||b>f->glyf.bytes)return -1;
 *at=f->glyf.at+a;*end=f->glyf.at+b;return 0;
}
static int32_t divide(int32_t n,uint32_t d){return n<0?-(int32_t)((0u-(uint32_t)n)/d):(int32_t)((uint32_t)n/d);}
static int point(const struct font *f,const struct transform *m,int32_t x,int32_t y,int32_t *ox,int32_t *oy){
 int64_t xx=(((int64_t)m->a*x+(int64_t)m->c*y)>>14)+m->x,yy=(((int64_t)m->b*x+(int64_t)m->d*y)>>14)+m->y;
 if(xx < -131071 || xx>131071 || yy < -131071 || yy>131071)return -1;
 *ox=divide((int32_t)xx*(int32_t)(f->pixels*64u),f->units);
 *oy=(int32_t)f->baseline*64-divide((int32_t)yy*(int32_t)(f->pixels*64u),f->units);return 0;
}
static int line(struct font *f,int32_t x0,int32_t y0,int32_t x1,int32_t y1){
 if(x0==x1&&y0==y1)return 0;
 if(f->edge_count==EDGES)return -1;
 struct edge *e=&f->edges[f->edge_count++];e->x0=x0;e->y0=y0;e->x1=x1;e->y1=y1;return 0;
}
static int curve(struct font *f,int32_t x0,int32_t y0,int32_t cx,int32_t cy,int32_t x1,int32_t y1,unsigned depth){
 int32_t dx=x0-2*cx+x1,dy=y0-2*cy+y1;
 if(depth==8||(dx>=-8&&dx<=8&&dy>=-8&&dy<=8))return line(f,x0,y0,x1,y1);
 int32_t a=(x0+cx)/2,b=(y0+cy)/2,c=(cx+x1)/2,d=(cy+y1)/2,mx=(a+c)/2,my=(b+d)/2;
 return curve(f,x0,y0,a,b,mx,my,depth+1)||curve(f,mx,my,c,d,x1,y1,depth+1)?-1:0;
}
static int outline(struct font *f,uint32_t glyph,const struct transform *m,unsigned depth){
 uint32_t at,end,n;if(depth>8||++f->shapes>256||location(f,glyph,&at,&end))return -1;
 if(at==end)return 0;
 if(end-at<10||half(f,at,&n))return -1;
 int contours=signed16(n);at+=10;
 if(contours<0){
  uint32_t flags,components=0;
  do{
   if(++components>128)return -1;
   uint32_t child,a,b;struct transform v={16384,0,0,16384,0,0},q;
   if(next_half(f,&at,end,&flags)||next_half(f,&at,end,&child)||!(flags&2u))return -1;
   if(flags&1u){if(next_half(f,&at,end,&a)||next_half(f,&at,end,&b))return -1;v.x=signed16(a);v.y=signed16(b);}
   else{if(next_byte(f,&at,end,&a)||next_byte(f,&at,end,&b))return -1;v.x=(int8_t)a;v.y=(int8_t)b;}
   if(flags&8u){if(next_half(f,&at,end,&a))return -1;v.a=v.d=signed16(a);}
   else if(flags&64u){if(next_half(f,&at,end,&a)||next_half(f,&at,end,&b))return -1;v.a=signed16(a);v.d=signed16(b);}
   else if(flags&128u){uint32_t c,d;if(next_half(f,&at,end,&a)||next_half(f,&at,end,&b)||next_half(f,&at,end,&c)||next_half(f,&at,end,&d))return -1;v.a=signed16(a);v.b=signed16(b);v.c=signed16(c);v.d=signed16(d);}
   q.a=(int32_t)(((int64_t)m->a*v.a+(int64_t)m->c*v.b)>>14);q.b=(int32_t)(((int64_t)m->b*v.a+(int64_t)m->d*v.b)>>14);
   q.c=(int32_t)(((int64_t)m->a*v.c+(int64_t)m->c*v.d)>>14);q.d=(int32_t)(((int64_t)m->b*v.c+(int64_t)m->d*v.d)>>14);
   q.x=(int32_t)(((int64_t)m->a*v.x+(int64_t)m->c*v.y)>>14)+m->x;q.y=(int32_t)(((int64_t)m->b*v.x+(int64_t)m->d*v.y)>>14)+m->y;
   if(q.a < -65536||q.a>65536||q.b < -65536||q.b>65536||q.c < -65536||q.c>65536||q.d < -65536||q.d>65536||q.x < -65536||q.x>65536||q.y < -65536||q.y>65536)return -1;
   if(outline(f,child,&q,depth+1))return -1;
  }while(flags&32u);
  return 0;
 }
 if(!contours)return 0;
 if(contours>(int)CONTOURS||end-at<(uint32_t)contours*2u+2u)return -1;
 for(int i=0;i<contours;i++){if(next_half(f,&at,end,&n)||n>=POINTS||(i&&n<=f->ends[i-1]))return -1;f->ends[i]=(uint16_t)n;}
 uint32_t points=(uint32_t)f->ends[contours-1]+1u,instructions;
 if(next_half(f,&at,end,&instructions)||instructions>end-at)return -1;
 at+=instructions;
 for(uint32_t i=0;i<points;){
  uint32_t flag,repeats=0;if(next_byte(f,&at,end,&flag))return -1;
  if((flag&8u)&&next_byte(f,&at,end,&repeats))return -1;
  if(repeats>=points-i)return -1;
  do{f->points[i++].on=(uint8_t)flag;}while(repeats--);
 }
 for(unsigned axis=0;axis<2;axis++){
  int32_t value=0;uint32_t short_bit=axis?4u:2u,same_bit=axis?32u:16u;
  for(uint32_t i=0;i<points;i++){
   uint32_t flags=f->points[i].on,delta=0;int32_t change=0;
   if(flags&short_bit){if(next_byte(f,&at,end,&delta))return -1;change=(flags&same_bit)?(int32_t)delta:-(int32_t)delta;}
   else if(!(flags&same_bit)){if(next_half(f,&at,end,&delta))return -1;change=signed16(delta);}
   value+=change;if(value < -32768||value>32767)return -1;
   if(axis)f->points[i].y=value;else f->points[i].x=value;
  }
 }
 for(uint32_t i=0;i<points;i++){struct point *p=&f->points[i];int32_t x,y;if(point(f,m,p->x,p->y,&x,&y))return -1;p->x=x;p->y=y;p->on&=1u;}
 unsigned first=0;
 for(int contour=0;contour<contours;contour++){
  unsigned last=f->ends[contour];struct point *a=&f->points[first],*b=&f->points[last];int32_t sx,sy,x,y;
  unsigned index;
  if(a->on){sx=a->x;sy=a->y;index=first+1;}
  else if(b->on){sx=b->x;sy=b->y;index=first;}
  else{sx=(a->x+b->x)/2;sy=(a->y+b->y)/2;index=first;}
  x=sx;y=sy;
  while(index<=last){
   struct point *p=&f->points[index];
   if(p->on){if(line(f,x,y,p->x,p->y))return -1;x=p->x;y=p->y;index++;}
   else{
    int32_t ex,ey;
    if(index==last){ex=sx;ey=sy;index++;}
    else{struct point *next=&f->points[index+1];if(next->on){ex=next->x;ey=next->y;index+=2;}else{ex=(p->x+next->x)/2;ey=(p->y+next->y)/2;index++;}}
    if(curve(f,x,y,p->x,p->y,ex,ey,0))return -1;
    x=ex;y=ey;
   }
  }
  if(line(f,x,y,sx,sy))return -1;
  first=last+1;
 }
 return 0;
}
const struct spmp_ttf_glyph *spmp_ttf_glyph(union spmp_ttf_font *memory,uint32_t cp){
 if(!memory)return 0;
 struct font *f=(struct font*)memory->opaque;if(!f->valid)return 0;
 for(unsigned i=0;i<CACHE;i++)if(f->cache[i].valid&&f->cache[i].cp==cp)return &f->cache[i].glyph;
 f->misses++;uint32_t glyph;if(glyph_index(f,cp,&glyph))return 0;if(!glyph&&cp!='?')return spmp_ttf_glyph(memory,'?');
 struct cached *c=&f->cache[f->cache_cursor++%CACHE];c->valid=0;
 uint32_t advance;if(half(f,f->hmtx.at+(glyph<f->metrics?glyph:f->metrics-1)*4,&advance))return 0;
 advance=(advance*f->pixels+f->units/2)/f->units;if(!advance)advance=1;if(advance>16)return 0;
 f->edge_count=0;f->shapes=0;const struct transform identity={16384,0,0,16384,0,0};if(outline(f,glyph,&identity,0))return 0;
 c->glyph.advance=(uint8_t)advance;c->glyph.width=0;c->glyph.height=SPMP_TTF_HEIGHT;
 for(unsigned y=0;y<SPMP_TTF_HEIGHT;y++){
  uint16_t bits=0;int32_t py=(int32_t)y*64+32;int changes[17]={0};
  for(unsigned i=0;i<f->edge_count;i++){
   const struct edge *e=&f->edges[i];if(e->y0==e->y1)continue;
   if(!((e->y0<=py&&e->y1>py)||(e->y1<=py&&e->y0>py)))continue;
   /* Find the last covered sample once per edge, rather than testing every
    * edge for every pixel. Binary search avoids a 64-bit division runtime. */
   unsigned lo=0,hi=16;int direction=e->y1>e->y0?1:-1;
   while(lo<hi){
    unsigned mid=(lo+hi)/2;int32_t px=(int32_t)mid*64+32;
    int64_t cross=(int64_t)(e->x1-e->x0)*(py-e->y0)-(int64_t)(e->y1-e->y0)*(px-e->x0);
    int covered=direction>0?cross>0:cross<0;
    if(covered)lo=mid+1;else hi=mid;
   }
   if(lo){changes[0]+=direction;changes[lo]-=direction;}
  }
  int winding=0;
  for(unsigned x=0;x<16;x++){
   winding+=changes[x];
   if(winding){bits|=(uint16_t)(1u<<x);if(c->glyph.width<x+1)c->glyph.width=(uint8_t)(x+1);}
  }
  c->glyph.rows[y]=bits;
 }
 c->cp=cp;c->valid=1;return &c->glyph;
}
unsigned spmp_ttf_cache_misses(const union spmp_ttf_font *memory){return memory?((const struct font*)memory->opaque)->misses:0;}
