#include "spmp_game_menu.h"
#include "spmp_theme.h"
#include "defos.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
int main(void){
 spmp_game_menu *m=calloc(1,sizeof *m);
 uint16_t *img=malloc(320u*240u*sizeof(uint16_t));
 assert(m&&img);
 spmp_menu_init(m,"sms","tests/game.sms",NULL,NULL,NULL,0);
 m->opened=1;m->settings_open=1;m->settings_selection=4;
 spmp_theme t;spmp_theme_default(&t);
 spmp_menu_draw(m,&t,img,320,240);
 FILE *f=fopen("build/stage16-settings.ppm","wb");assert(f);
 fprintf(f,"P6\n320 240\n255\n");
 for(unsigned i=0;i<320u*240u;i++){
  uint16_t x=img[i];unsigned char rgb[3]={(unsigned char)((((x>>11)&31)*255)/31),
    (unsigned char)((((x>>5)&63)*255)/63),
    (unsigned char)(((x&31)*255)/31)};
  assert(fwrite(rgb,1,3,f)==3);
 }
 assert(!fclose(f));free(img);free(m);return 0;
}
/* The link wrapper for defos_render expects this but does not call it. */
const char *defos_kind_name(defos_kind kind){(void)kind;return "OTHER";}
