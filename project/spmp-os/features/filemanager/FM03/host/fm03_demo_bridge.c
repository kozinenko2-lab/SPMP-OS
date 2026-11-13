/* Host-only bridge for C RGB565 renderer screenshot checks. */
#include "spmp_fm03.h"
#include "fm03_posix.h"
#include "fm01_posix.h"
#include <stdlib.h>
#include <string.h>
typedef struct {
    fm01_posix browse;
    fm03_posix props;
    fm01_model browser;
    fm03_model sheet;
} demo;
void *fm03_demo_open(const char *root){
    fm03_backend backend;demo *d=(demo*)calloc(1,sizeof(*d));
    if(!d)return NULL;
    d->browse.rootfd=-1;d->props.rootfd=-1;
    if(fm01_posix_init(&d->browse,root)!=0)goto fail;
    if(fm03_posix_init(&d->props,root)!=0)goto fail;
    fm01_init(&d->browser,fm01_posix_list,&d->browse);
    d->browser.sd_present=1;
    fm03_posix_backend(&backend);
    fm03_init(&d->sheet,&d->browser,&backend,&d->props);
    return d;
fail:
    fm03_posix_close(&d->props);fm01_posix_close(&d->browse);free(d);return NULL;
}
void fm03_demo_close(void *data){
    demo *d=(demo*)data;if(!d)return;
    fm03_close(&d->sheet);fm03_posix_close(&d->props);fm01_posix_close(&d->browse);free(d);
}
int fm03_demo_paths(void *data,const char *left,const char *right){
    demo *d=(demo*)data;
    if(!d||!left||!right||strlen(left)>=FM01_PATH_MAX||strlen(right)>=FM01_PATH_MAX)return -1;
    memcpy(d->browser.panels[0].path,left,strlen(left)+1);
    memcpy(d->browser.panels[1].path,right,strlen(right)+1);
    if(fm01_refresh(&d->browser,0)<0)return -2;
    if(fm01_refresh(&d->browser,1)<0)return -2;
    return 0;
}
int fm03_demo_props(void *data,const char *parent,const char *name){
    demo *d=(demo*)data;
    return d?fm03_open(&d->sheet,parent,name):-1;
}
int fm03_demo_steps(void *data,unsigned ticks,unsigned budget){
    demo *d=(demo*)data;unsigned i;
    if(!d)return -1;
    for(i=0;i<ticks && d->sheet.state==FM03_SCANNING;i++)fm03_tick(&d->sheet,budget);
    return (int)d->sheet.state;
}
void fm03_demo_locale(void *data,unsigned locale){demo *d=(demo*)data;if(d)d->browser.locale=(uint8_t)(locale?1:0);}
void fm03_demo_draw(void *data,uint16_t *frame,unsigned pitch,fm01_font_draw font,void *font_ctx,
    const fm01_palette *palette,const fm01_status *status){
    demo *d=(demo*)data;fm01_font f={font,font_ctx};
    if(!d)return;
    fm01_render_overlay(&d->browser,frame,pitch,palette,&f,status);
    fm03_render_overlay(&d->sheet,frame,pitch,palette,&f);
}
