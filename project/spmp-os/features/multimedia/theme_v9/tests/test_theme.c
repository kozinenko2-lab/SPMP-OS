#include "spmp_ui_theme_v7.h"
#include "spmp_ui_builtin_glyphs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
static uint8_t *readall(const char*name,size_t*out){FILE*f=fopen(name,"rb");assert(f);assert(fseek(f,0,SEEK_END)==0);long n=ftell(f);assert(n>=0 && n<1000000);rewind(f);uint8_t*p=malloc((size_t)n);assert(p);assert(fread(p,1,(size_t)n,f)==(size_t)n);fclose(f);*out=(size_t)n;return p;}
static int reader(void*ctx,uint32_t off,void*dest,uint32_t len){FILE*f=ctx;return fseek(f,off,SEEK_SET)?-1:(int)fread(dest,1,len,f);}
static int short_reader(void*ctx,uint32_t off,void*dest,uint32_t len){if(off>32)return -1;return reader(ctx,off,dest,len);}
int main(int argc,char**argv){assert(argc==2);const char*base=argv[1];char path[512];size_t a,b,c;
 const uint8_t*font=spmp_ui_builtin_glyphs(&a);
 snprintf(path,sizeof(path),"%s/shared/icons.spi",base);uint8_t*icons=readall(path,&b);
 assert(spmp_ui_validate_font(font,a)==0 && spmp_ui_validate_icons(icons,b)==0);
 unsigned tests=0;for(unsigned j=0;j<5;j++){
  const char*names[]={"glamour","nature","racing","comics","action"};
  snprintf(path,sizeof(path),"%s/%s/style.spth",base,names[j]);uint8_t*d=readall(path,&c);
  spmp_ui_style style;assert(spmp_ui_parse_style(d,c,&style)==0);tests++;
  d[0]^=0xff;assert(spmp_ui_parse_style(d,c,&style)<0);d[0]^=0xff;free(d);
  snprintf(path,sizeof(path),"%s/%s/background.spbg",base,names[j]);FILE*bg=fopen(path,"rb");assert(bg);assert(spmp_ui_validate_background(reader,bg)==0);
  spmp_ui_item menu[]={{"Игры",0},{"Музыка",1},{"Видео",2},{"Фото",3},
      {"Радио",4},{"Файлы",5},{"Настройки",6},{"Темы",7}};
  spmp_ui_state st;spmp_ui_state_init(&st,SPMP_UI_LAYOUT_LIST,8);
  spmp_ui_status status={255,255,50,1};uint16_t*fb=malloc(320*240*2);uint8_t scratch[2560];assert(fb);
  assert(spmp_ui_render(fb,320,reader,bg,scratch,sizeof scratch,&style,font,a,icons,b,menu,8,&status,&st)==0);tests++;
  assert(spmp_ui_key_event(&st,SPMP_UI_DOWN)==SPMP_UI_SFX_MOVE && st.selected==1);
  assert(spmp_ui_key_event(&st,SPMP_UI_ACCEPT)==SPMP_UI_SFX_ACCEPT);
  assert(spmp_ui_key_event(&st,SPMP_UI_TOGGLE_LAYOUT)==SPMP_UI_SFX_MOVE && st.layout==1);
  assert(spmp_ui_key_event(&st,SPMP_UI_RIGHT)==SPMP_UI_SFX_MOVE && st.selected==2);
  assert(spmp_ui_key_event(&st,SPMP_UI_DOWN)==SPMP_UI_SFX_MOVE && st.selected==5);
  assert(spmp_ui_key_event(&st,SPMP_UI_RIGHT)==SPMP_UI_SFX_NONE);
  status.sd_present=0;spmp_ui_animate(&st);
  assert(spmp_ui_render(fb,320,reader,bg,scratch,sizeof scratch,&style,font,a,icons,b,menu,8,&status,&st)==0);tests++;
  assert(spmp_ui_render(fb,320,short_reader,bg,scratch,sizeof scratch,&style,font,a,icons,b,menu,8,&status,&st)>0);tests++;
  assert(spmp_ui_render_overlay(fb,320,&style,font,a,icons,b,menu,8,&status,&st)==0);tests++;
  int16_t pcm[SPMP_UI_SOUND_FRAMES*2];unsigned nz=0;
  assert(spmp_ui_sfx_generate(SPMP_UI_SFX_ACCEPT,pcm,SPMP_UI_SOUND_FRAMES)==SPMP_UI_SOUND_FRAMES);
  for(unsigned k=0;k<SPMP_UI_SOUND_FRAMES*2;k++)if(pcm[k])nz++;
  assert(nz>500);tests++;free(fb);fclose(bg);
 }
 free(icons);printf("PASS %u theme validations across 5 packs\n",tests);return 0;}
