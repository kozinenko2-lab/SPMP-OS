#!/usr/bin/env python3
"""Generate editable label-free layers, text-free icons, theme catalogue C API and localizations."""
from pathlib import Path
from PIL import Image,ImageDraw
import struct,json
ROOT=Path(__file__).resolve().parents[1]
THEMES=json.loads((ROOT/'catalog.json').read_text(encoding='utf8'))['themes']

def rgba(hex_code,alpha):
 return tuple(int(hex_code[i:i+2],16) for i in (1,3,5))+(alpha,)

def overlays():
 for t in THEMES:
  directory=ROOT/'themes'/t['id']; pal=json.loads((directory/'theme.json').read_text(encoding='utf8'))['colors']
  accent,base=rgba(pal['accent'],220),rgba(pal['panel'],165)
  # UI EXAMPLE ASSET ONLY: never contains localized text or raster icons.
  im=Image.new('RGBA',(320,240),(0,0,0,0));d=ImageDraw.Draw(im,'RGBA')
  d.rectangle((0,0,319,23),fill=rgba(pal['panel'],235));d.line((0,23,319,23),fill=accent,width=1)
  d.rectangle((0,26,149,215),fill=base)
  for y in range(30,206,22):d.line((5,y+20,145,y+20),fill=rgba(pal['border'],85),width=1)
  d.rounded_rectangle((4,30,146,51),radius=4,fill=rgba(pal['sel'],220),outline=accent,width=1)
  d.rectangle((0,216,319,239),fill=rgba(pal['panel'],235));d.line((0,216,319,216),fill=accent,width=1)
  im.save(directory/'ui_list_panels.png',optimize=True)
  im=Image.new('RGBA',(320,240),(0,0,0,0));d=ImageDraw.Draw(im,'RGBA')
  d.rectangle((0,0,319,23),fill=rgba(pal['panel'],235));d.line((0,23,319,23),fill=accent,width=1)
  for j in range(8):
   x=10+j%3*104;y=30+j//3*61
   d.rounded_rectangle((x,y,x+91,y+54),radius=5,fill=base,outline=accent if j==0 else rgba(pal['border'],130),width=1)
  d.rectangle((0,216,319,239),fill=rgba(pal['panel'],235));d.line((0,216,319,216),fill=accent,width=1)
  im.save(directory/'ui_grid_panels.png',optimize=True)


def icons():
 icon_path=ROOT/'themes'/'shared'/'icons.spi';data=icon_path.read_bytes()
 magic,n,w,h=struct.unpack('<4sHBB',data[:8]);assert magic==b'SPI1' and n==8 and w==h==18
 names=['games','music','video','photos','radio','files','settings','themes']
 folder=ROOT/'themes'/'shared'/'icons';folder.mkdir(parents=True,exist_ok=True)
 for k,name in enumerate(names):
  im=Image.new('RGBA',(w,h),(0,0,0,0))
  for y in range(h):
   row=int.from_bytes(data[8+k*h*3+y*3:8+k*h*3+y*3+3],'little')
   for x in range(w):
    if row>>x&1:im.putpixel((x,y),(255,255,255,255))
  im.save(folder/(name+'.png'))
 sheet=Image.new('RGBA',(8*24,24),(0,0,0,0))
 for k,name in enumerate(names):sheet.alpha_composite(Image.open(folder/(name+'.png')),(k*24+3,3))
 sheet.save(ROOT/'themes'/'shared'/'icons_symbol_only.png')


def catalog_api():
 include=ROOT/'include';src=ROOT/'src'
 (include/'spmp_theme_catalog_v9.h').write_text('''#ifndef SPMP_THEME_CATALOG_V9_H
#define SPMP_THEME_CATALOG_V9_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Stable catalog IDs, no libc, no heap. Labels are not in image resources. */
#define SPMP_THEME_V9_COUNT 34
size_t spmp_theme_v9_count(void);
const char *spmp_theme_v9_id(size_t index);
const char *spmp_theme_v9_category(size_t index);
int spmp_theme_v9_find(const char *id);
/* Append slash-separated relative path: themes/<id>/<leaf>. Reject unknown IDs/leaves. */
int spmp_theme_v9_path(size_t index,const char *leaf,char *out,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
''')
 categories=[t['category'] for t in THEMES];ids=[t['id'] for t in THEMES]
 txt='''#include "spmp_theme_catalog_v9.h"
static const char * const ids[SPMP_THEME_V9_COUNT]={\n'''+','.join('"'+x+'"' for x in ids)+'''};
static const char * const cats[SPMP_THEME_V9_COUNT]={\n'''+','.join('"'+x+'"' for x in categories)+'''};
static int eq(const char*a,const char*b){if(!a||!b)return 0;while(*a&&*b){if(*a++!=*b++)return 0;}return !*a&&!*b;}
size_t spmp_theme_v9_count(void){return SPMP_THEME_V9_COUNT;}
const char *spmp_theme_v9_id(size_t n){return n<SPMP_THEME_V9_COUNT?ids[n]:0;}
const char *spmp_theme_v9_category(size_t n){return n<SPMP_THEME_V9_COUNT?cats[n]:0;}
int spmp_theme_v9_find(const char *id){for(size_t i=0;i<SPMP_THEME_V9_COUNT;i++)if(eq(ids[i],id))return (int)i;return -1;}
int spmp_theme_v9_path(size_t n,const char *leaf,char*out,size_t cap){
 if(!out||n>=SPMP_THEME_V9_COUNT||!leaf)return -1;
 if(!eq(leaf,"background.spbg")&&!eq(leaf,"style.spth")&&!eq(leaf,"theme.json"))return -2;
 const char*parts[]={"themes/",ids[n],"/",leaf};size_t pos=0;
 for(unsigned k=0;k<4;k++)for(const char*p=parts[k];*p;p++){
  if(pos>=cap||pos+1>=cap){if(cap)out[0]=0;return -3;}
  out[pos++]=*p;
 }
 out[pos]=0;return (int)pos;
}
'''
 (src/'spmp_theme_catalog_v9.c').write_text(txt)
 (ROOT/'tests'/'test_catalog_v9.c').write_text('''#include "spmp_theme_catalog_v9.h"
#include <assert.h>
#include <string.h>
int main(void){char p[128];assert(spmp_theme_v9_count()==34);
 for(size_t i=0;i<spmp_theme_v9_count();i++){
  const char*id=spmp_theme_v9_id(i);assert(id&&spmp_theme_v9_category(i));
  assert(spmp_theme_v9_find(id)==(int)i);
  assert(spmp_theme_v9_path(i,"background.spbg",p,sizeof p)>0);
  assert(!strncmp(p,"themes/",7));}
 assert(spmp_theme_v9_find("../../dev/null")==-1);
 assert(spmp_theme_v9_path(999,"background.spbg",p,sizeof p)<0);
 assert(spmp_theme_v9_path(0,"../../etc/passwd",p,sizeof p)<0);
 assert(spmp_theme_v9_path(0,"background.spbg",p,8)<0);
 return 0;}
''')

def localizations():
 p=ROOT/'locales';p.mkdir(exist_ok=True)
 labels={'ru':['Игры','Музыка','Видео','Фото','Радио','Файлы','Настройки','Темы'],
         'en':['Games','Music','Video','Photos','Radio','Files','Settings','Themes']}
 for lang,strings in labels.items():
  (p/(lang+'.json')).write_text(json.dumps({'language':lang,'menu_keys':['games','music','video','photos','radio','files','settings','themes'],'labels':strings},ensure_ascii=False,indent=2)+'\n',encoding='utf8')
 f=ROOT/'src'/'theme_preview_host.c';s=f.read_text()
 if 'static const spmp_ui_item menu_en[]=' not in s:
  s=s.replace('static const spmp_ui_item menu[]={','static const spmp_ui_item menu_ru[]={').replace('int spmp_ui_v7_preview_cli(int argc,char**argv){','''static const spmp_ui_item menu_en[]={
 {"Games",0},{"Music",1},{"Video",2},{"Photos",3},
 {"Radio",4},{"Files",5},{"Settings",6},{"Themes",7}
};
int spmp_ui_v7_preview_cli(int argc,char**argv){''')
  s=s.replace('const char*root=argv[1],*id=argv[2];int grid=!strcmp(argv[3],"grid");','const char*root=argv[1],*id=argv[2];int grid=!strcmp(argv[3],"grid");\n const spmp_ui_item *menu=(argc>9 && !strcmp(argv[9],"en"))?menu_en:menu_ru;')
 s=s.replace('OUTPUT.ppm [selected] [sd=0|1] [volume=0..100]','OUTPUT.ppm [selected] [sd=0|1] [volume=0..100] [overlay=black|white] [lang=ru|en]')
 f.write_text(s)

if __name__=='__main__':
 overlays();icons();catalog_api();localizations()
 print('34 text-free UI panel layers ×2, 8 symbolic icon PNG, runtime theme catalog, RU/EN localization')
