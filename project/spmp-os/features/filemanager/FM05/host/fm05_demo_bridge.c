/* SPDX-License-Identifier: MIT - Host-only preview adapter. */
#include "spmp_fm05.h"
#include <stdlib.h>
#include <string.h>
typedef struct {fm05_doc doc;fm04_model keyboard;char sample[1024];unsigned len;uint64_t revision;} demo;
static int load(void *ctx,const char *path,char *data,unsigned cap,unsigned *n,uint64_t *revision){
    demo*d=(demo*)ctx;(void)path;
    if(d->len>cap)return FM05_ERR_TOO_LARGE;
    memcpy(data,d->sample,d->len);*n=d->len;*revision=d->revision;return 0;
}
static int save(void *ctx,const char *path,const char *data,unsigned size,uint64_t expected,uint64_t *updated){
    demo*d=(demo*)ctx;(void)path;
    if(size>=sizeof(d->sample))return FM05_ERR_TOO_LARGE;
    if(expected!=d->revision)return FM05_ERR_CONFLICT;
    memcpy(d->sample,data,size);d->sample[size]=0;d->len=size;*updated=++d->revision;return 0;
}
void *fm05_demo_create(const char *path,const char *sample,unsigned locale,unsigned state){
    demo*d=(demo*)calloc(1,sizeof(*d));size_t n;
    if(!d||!path||!sample){free(d);return NULL;}
    n=strlen(sample);if(n>=sizeof(d->sample)){free(d);return NULL;}
    memcpy(d->sample,sample,n+1u);d->len=(unsigned)n;d->revision=1;
    fm05_init(&d->doc,(fm05_fs){load,save,d});
    if(fm05_open(&d->doc,path,locale)!=0){free(d);return NULL;}
    if(state==1u){fm05_line_end(&d->doc);(void)fm05_insert(&d->doc," *",2);}
    if(state==2u){(void)fm05_insert(&d->doc,"!",1);(void)fm05_request_close(&d->doc);}
    if(state==3u){
        (void)fm05_open_keyboard(&d->doc,&d->keyboard,FM05_INPUT_TEXT);
        d->keyboard.layout=FM04_RU;
    }
    return d;
}
void fm05_demo_destroy(void *ptr){free(ptr);}
void fm05_demo_draw(void *ptr,uint16_t *fb,unsigned pitch,
    fm01_font_draw draw,void *font_context,const fm01_palette *colors,unsigned with_keyboard){
    demo*d=(demo*)ptr;fm01_font f={draw,font_context};
    if(!d)return;
    fm05_render(&d->doc,fb,pitch,colors,&f);
    if(with_keyboard)fm04_render_overlay(&d->keyboard,fb,pitch,colors,&f);
}
