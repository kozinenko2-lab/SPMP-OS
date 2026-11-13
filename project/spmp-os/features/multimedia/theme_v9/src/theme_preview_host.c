#include "spmp_ui_theme_v7.h"
#include "spmp_ui_builtin_glyphs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
static unsigned char *whole(const char*path,size_t*n){FILE*f=fopen(path,"rb");if(!f)return 0;
 if(fseek(f,0,SEEK_END)){fclose(f);return 0;}long l=ftell(f);if(l<0||l>400000){fclose(f);return 0;}
 rewind(f);unsigned char*p=malloc((size_t)l+1);if(!p){fclose(f);return 0;}
 if(fread(p,1,(size_t)l,f)!=(size_t)l){fclose(f);free(p);return 0;}
 fclose(f);*n=(size_t)l;return p;}
static int read_at(void*ctx,uint32_t off,void*dst,uint32_t len){FILE*f=ctx;
 if(fseek(f,(long)off,SEEK_SET))return -1;
 return (int)fread(dst,1,len,f);}
static int saveppm(const char*path,const uint16_t*fb){FILE*f=fopen(path,"wb");if(!f)return -1;
 fprintf(f,"P6\n320 240\n255\n");for(unsigned i=0;i<320*240;i++){
  unsigned p=fb[i];unsigned char rgb[3]={(unsigned char)(((p>>11)&31)*255/31),
   (unsigned char)(((p>>5)&63)*255/63),(unsigned char)((p&31)*255/31)};
  if(fwrite(rgb,1,3,f)!=3){fclose(f);return -1;}}
 return fclose(f);
}
static const spmp_ui_item menu_ru[]={
 {"Игры",0},{"Музыка",1},{"Видео",2},{"Фото",3},
 {"Радио",4},{"Файлы",5},{"Настройки",6},{"Темы",7}
};
static const spmp_ui_item menu_en[]={
 {"Games",0},{"Music",1},{"Video",2},{"Photos",3},
 {"Radio",4},{"Files",5},{"Settings",6},{"Themes",7}
};
int spmp_ui_v7_preview_cli(int argc,char**argv){
 if(argc<5){fprintf(stderr,"Usage: %s ASSET_ROOT THEME list|grid OUTPUT.ppm [selected] [sd=0|1] [volume=0..100] [overlay=black|white] [lang=ru|en]\n",argv[0]);return 2;}
 const char*root=argv[1],*id=argv[2];int grid=!strcmp(argv[3],"grid");
 const spmp_ui_item *menu=(argc>9 && !strcmp(argv[9],"en"))?menu_en:menu_ru;
 char path[512];size_t fn=0,in=0,sn=0;
 const uint8_t*font=spmp_ui_builtin_glyphs(&fn);
 snprintf(path,sizeof(path),"%s/shared/icons.spi",root);uint8_t*icons=whole(path,&in);
 snprintf(path,sizeof(path),"%s/%s/style.spth",root,id);uint8_t*style=whole(path,&sn);
 spmp_ui_style pal;
 if(!font||!icons||!style||spmp_ui_parse_style(style,sn,&pal)){fprintf(stderr,"invalid theme assets\n");return 1;}
 snprintf(path,sizeof(path),"%s/%s/background.spbg",root,id);FILE*bg=fopen(path,"rb");if(!bg){perror("bg");return 1;}
 uint16_t*fb=calloc(320*240,sizeof(uint16_t));uint8_t*scratch=malloc(SPMP_UI_BG_SCRATCH);
 spmp_ui_state ui;spmp_ui_state_init(&ui,(uint8_t)grid,8);
 if(argc>5){unsigned selected=(unsigned)atoi(argv[5]);if(selected<8)ui.selected=(uint8_t)selected;}
 spmp_ui_status status={15,24,(uint8_t)(argc>7?atoi(argv[7]):75),(uint8_t)(argc>6?atoi(argv[6]):1)};
 int ret=0;
 if(argc>8 && (!strcmp(argv[8],"white")||!strcmp(argv[8],"black"))){
   uint16_t base=!strcmp(argv[8],"white")?0xffff:0;
   for(unsigned i=0;i<320u*240u;i++)fb[i]=base;
   ret=spmp_ui_render_overlay(fb,320,&pal,font,fn,icons,in,menu,8,&status,&ui);
 }else ret=spmp_ui_render(fb,320,read_at,bg,scratch,SPMP_UI_BG_SCRATCH,&pal,
                       font,fn,icons,in,menu,8,&status,&ui);
 if(ret){fprintf(stderr,"theme background could not be read\n");return 1;}
 if(saveppm(argv[4],fb)){perror("ppm");return 1;}
 printf("Rendered %s / %s : %s\n",id,grid?"grid":"list",argv[4]);
 fclose(bg);free(fb);free(scratch);free(icons);free(style);return 0;
}

#ifndef SPMP_THEME_V7_NO_MAIN
int main(int argc,char**argv){return spmp_ui_v7_preview_cli(argc,argv);}
#endif
