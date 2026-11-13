/* Optional POSIX demo bridge. Not linked into the ARMv5 firmware library. */
#include "fm01_posix.h"
#include <stdlib.h>
#include <string.h>

typedef struct {fm01_posix fs;fm01_model model;} fm01_demo;

void *fm01_demo_open(const char *root) {
    fm01_demo *d=(fm01_demo*)calloc(1,sizeof(*d));
    if(!d)return 0;
    d->fs.rootfd=-1;
    if(fm01_posix_init(&d->fs,root)){free(d);return 0;}
    fm01_init(&d->model,fm01_posix_list,&d->fs);
    return d;
}
void fm01_demo_close(void *handle) {
    fm01_demo *d=(fm01_demo*)handle;
    if(d){fm01_posix_close(&d->fs);free(d);}
}
int fm01_demo_set_paths(void *handle,const char *left,const char *right) {
    fm01_demo *d=(fm01_demo*)handle;const char *items[2]={left,right};unsigned i;
    if(!d)return -1;
    for(i=0;i<2;i++) {
        size_t n;if(!items[i])continue;
        n=strlen(items[i]);if(n>=FM01_PATH_MAX)return -2;
        memcpy(d->model.panels[i].path,items[i],n+1);
        d->model.panels[i].cursor=0;d->model.panels[i].depth=0;
        if(fm01_refresh(&d->model,i))return -3;
    }
    return 0;
}
void fm01_demo_keys(void *handle,unsigned pressed) {
    fm01_demo *d=(fm01_demo*)handle;
    if(d)fm01_buttons(&d->model,pressed);
}
void fm01_demo_locale(void *handle,unsigned locale) {
    fm01_demo *d=(fm01_demo*)handle;if(d)d->model.locale=(uint8_t)(locale?1:0);
}
void fm01_demo_render(void *handle,uint16_t *px,unsigned pitch,
    fm01_font_draw fn,void *font_ctx,const fm01_palette *palette,
    const fm01_status *status) {
    fm01_demo *d=(fm01_demo*)handle;fm01_font font={fn,font_ctx};
    if(d)fm01_render(&d->model,px,pitch,palette,&font,status);
}
const fm01_model *fm01_demo_model(void *handle) {
    fm01_demo *d=(fm01_demo*)handle;return d?&d->model:0;
}

void fm01_demo_overlay(void *handle,uint16_t *px,unsigned pitch,
    fm01_font_draw fn,void *font_ctx,const fm01_palette *palette,
    const fm01_status *status) {
    fm01_demo *d=(fm01_demo*)handle;fm01_font font={fn,font_ctx};
    if(d)fm01_render_overlay(&d->model,px,pitch,palette,&font,status);
}
