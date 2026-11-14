#include "display.h"
#include "spmp_ui_builtin_glyphs.h"
#include "spmp_ttf.h"
static union spmp_ttf_font system_font;
static struct {const struct input_services *io;int fd;uint32_t size,page,count;uint8_t buffer[2048];} font_reader;
uint32_t spmp_files_font_mode;
static int font_read(void *user,uint32_t at,void *output,uint32_t n){
 (void)user;uint8_t *out=output;
 if(at>font_reader.size||n>font_reader.size-at)return -1;
 while(n){
  uint32_t page=at&~2047u;
  if(!font_reader.count||font_reader.page!=page){
   uint32_t count=font_reader.size-page;if(count>2048)count=2048;
   if(font_reader.io->seek(font_reader.fd,(int32_t)page,0)!=(int32_t)page)return -1;
   uint32_t done=0;
   typedef int (*read_fn)(int,void *,uint32_t);
   while(done<count){int got=((read_fn)(uintptr_t)0x2eb5e0)(font_reader.fd,font_reader.buffer+done,count-done);if(got<=0||(uint32_t)got>count-done)return -1;done+=(uint32_t)got;}
   font_reader.page=page;font_reader.count=count;
  }
  uint32_t offset=at-page,count=font_reader.count-offset;if(count>n)count=n;
  for(uint32_t i=0;i<count;i++)out[i]=font_reader.buffer[offset+i];
  out+=count;at+=count;n-=count;
 }
 return 0;
}
void spmp_files_font_open(const struct input_services *io){
 static const char path[]="/fat20a0/hda0/SPMP-OS/System/Fonts/Roboto-Regular.ttf";
 typedef int (*stat_fn)(const char *,void *);uint32_t st[8]={0};
 font_reader.fd=-1;font_reader.count=0;font_reader.io=io;spmp_files_font_mode=0;
 if(((stat_fn)(uintptr_t)0x2a6504)(path,st)||!(st[0]&8u)||st[4]<12||st[4]>4u*1024u*1024u)return;
 int fd=io->open(path,1,0);if(fd<0)return;font_reader.fd=fd;font_reader.size=st[4];
 struct spmp_ttf_io reader={0,font_reader.size,font_read};
 if(spmp_ttf_open(&system_font,&reader,13)){io->close(fd);font_reader.fd=-1;return;}
 spmp_files_font_mode=1;
}
void spmp_files_font_close(void){if(font_reader.fd>=0){font_reader.io->close(font_reader.fd);font_reader.fd=-1;}}
int spmp_files_rect(void *ctx,unsigned x,unsigned y,unsigned w,unsigned h,uint32_t rgb){
 const struct input_services *io=ctx;if(x>=320||y>=240||w>320-x||h>240-y)return -1;
 struct mmi_rgb888 c={(uint8_t)(rgb>>16),(uint8_t)(rgb>>8),(uint8_t)rgb};
 struct mmi_rect_s16 r={(int16_t)x,(int16_t)y,(uint16_t)w,(uint16_t)h};
 return io->graphics.set_color(&c)||io->graphics.fill(&r)?-1:0;
}
static unsigned u16(const uint8_t *p){return (unsigned)p[0]|((unsigned)p[1]<<8);}
static const uint8_t *glyph(const uint8_t *font,unsigned cp){
 unsigned lo=0,hi=u16(font+4);while(lo<hi){unsigned mid=lo+(hi-lo)/2;const uint8_t *p=font+8+mid*36u;unsigned c=u16(p);if(c==cp)return p;if(c<cp)lo=mid+1;else hi=mid;}return 0;
}
static unsigned next(const char **text){
 const uint8_t *p=(const uint8_t*)*text;unsigned c=*p;(*text)++;
 if(c<128)return c;
 if(c>=0xc2&&c<=0xdf&&p[1]>=0x80&&p[1]<=0xbf){*text+=1;return ((c&31u)<<6)|(p[1]&63u);}
 if(c>=0xe0&&c<=0xef&&p[1]>=0x80&&p[1]<=0xbf&&p[2]>=0x80&&p[2]<=0xbf){*text+=2;return ((c&15u)<<12)|((p[1]&63u)<<6)|(p[2]&63u);}
 return '?';
}
int spmp_files_text(void *ctx,unsigned x,unsigned y,const char *s,unsigned maxw,uint32_t rgb){
 const struct input_services *io=ctx;
 size_t size;const uint8_t *font=spmp_ui_builtin_glyphs(&size);
 if(!s||size<8||size!=8u+u16(font+4)*36u||maxw>320||x>320-maxw)return -1;
 const struct mmi_rgb888 color={(uint8_t)(rgb>>16),(uint8_t)(rgb>>8),(uint8_t)rgb};
 if(io->graphics.set_color(&color))return -1;
 unsigned start=x;
 for(unsigned count=0;*s&&count<256;count++){
  uint32_t cp=next(&s);const struct spmp_ttf_glyph *ttf=spmp_files_font_mode?spmp_ttf_glyph(&system_font,cp):0;
  const uint8_t *g=ttf?0:glyph(font,cp);if(!ttf&&!g)g=glyph(font,'?');if(!ttf&&!g)return -1;
  unsigned advance=ttf?ttf->advance:g[2],width=ttf?ttf->width:g[3],height=ttf?ttf->height:16;
  if(x-start+advance>maxw)break;
  if(width>maxw-(x-start))width=maxw-(x-start);
  for(unsigned row=0;row<height&&y+row<240;row++){
   unsigned bits=ttf?ttf->rows[row]:u16(g+4+row*2),xx=0;
   while(xx<width){
    if(!(bits&(1u<<xx))){xx++;continue;}
    unsigned begin=xx;while(xx<width&&(bits&(1u<<xx)))xx++;
    const struct mmi_rect_s16 r={(int16_t)(x+begin),(int16_t)(y+row),(uint16_t)(xx-begin),1};
    if(io->graphics.fill(&r))return -1;
   }
  }
  x+=advance;
 }
 return 0;
}
