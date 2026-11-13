/* Host demonstration only: actual QVGA C renderer and external font callback. */
#include "spmp_fm04.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
void *fm04_demo_create(const char *initial,unsigned locale) {
    fm04_model *k=(fm04_model*)calloc(1,sizeof(*k));
    if(!k)return NULL;
    if(fm04_open(k,initial,FM04_FILENAME,locale)){free(k);return NULL;}
    return k;
}
void fm04_demo_destroy(void *data){free(data);}
void fm04_demo_layout(void *data,unsigned layout,unsigned focus) {
    fm04_model *k=(fm04_model*)data;
    if(!k)return;
    if(layout<3)k->layout=(uint8_t)layout;
    if(focus<FM04_KEYS)k->selection=(uint8_t)focus;
}
void fm04_demo_draw(void *data,uint16_t *px,unsigned pitch,fm01_font_draw fn,void *font_ctx,
    const fm01_palette *palette){
    fm04_model *k=(fm04_model*)data;fm01_font font={fn,font_ctx};
    if(k)fm04_render_overlay(k,px,pitch,palette,&font);
}
