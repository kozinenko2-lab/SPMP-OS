/* SPDX-License-Identifier: MIT. Optional desktop preview/UI integration only. */
#include "fm02_posix.h"
#include "fm01_posix.h"
#include <stdlib.h>
#include <string.h>
typedef struct {fm01_posix ro;fm02_posix rw;fm01_model browser;fm02_model actions;} fm02_demo;
void *fm02_demo_open(const char *root){
    fm02_demo *d=(fm02_demo*)calloc(1,sizeof(*d));
    if(!d)return NULL;
    d->ro.rootfd=-1;d->rw.rootfd=-1;
    if(fm01_posix_init(&d->ro,root)){free(d);return NULL;}
    if(fm02_posix_init(&d->rw,root)){fm01_posix_close(&d->ro);free(d);return NULL;}
    fm01_init(&d->browser,fm01_posix_list,&d->ro);
    fm02_init(&d->actions,&d->browser,fm02_posix_action,&d->rw);
    return d;
}
void fm02_demo_close(void *handle){fm02_demo *d=(fm02_demo*)handle;
    if(d){fm02_posix_close(&d->rw);fm01_posix_close(&d->ro);free(d);}
}
int fm02_demo_paths(void *handle,const char *left,const char *right){
    fm02_demo *d=(fm02_demo*)handle;const char *v[]={left,right};unsigned i;
    if(!d)return -1;
    for(i=0;i<2;i++){size_t n;if(!v[i])continue;n=strlen(v[i]);if(n>=FM01_PATH_MAX)return -2;
        memcpy(d->browser.panels[i].path,v[i],n+1);
        d->browser.panels[i].cursor=0;d->browser.panels[i].depth=0;
        if(fm01_refresh(&d->browser,i))return -3;
    }
    fm02_clear_marks(&d->actions);
    return 0;
}
void fm02_demo_locale(void *handle,unsigned locale){fm02_demo *d=(fm02_demo*)handle;
    if(d)d->browser.locale=(uint8_t)!!locale;}
void fm02_demo_keys(void *handle,unsigned keys){fm02_demo *d=(fm02_demo*)handle;
    if(d)fm02_buttons(&d->actions,keys);}
int fm02_demo_action(void *handle,unsigned action,const char *rename){fm02_demo *d=(fm02_demo*)handle;int rc;
    if(!d)return -1;
    rc=fm02_prepare(&d->actions,(fm02_action)action,rename);
    if(!rc && action!=FM02_DELETE)rc=fm02_commit(&d->actions,1);
    return rc;
}
void fm02_demo_draw(void *handle,uint16_t *rgb565,unsigned pitch,
    fm01_font_draw draw,void *ctx,const fm01_palette *colors,const fm01_status *status){
    fm02_demo *d=(fm02_demo*)handle;fm01_font f={draw,ctx};if(!d)return;
    fm01_render_overlay(&d->browser,rgb565,pitch,colors,&f,status);
    fm02_render_status(&d->actions,rgb565,pitch,colors,&f);
}
unsigned fm02_demo_marks(void *handle){fm02_demo *d=(fm02_demo*)handle;return d?d->actions.marks_count:0;}
unsigned fm02_demo_state(void *handle){fm02_demo *d=(fm02_demo*)handle;return d?d->actions.state:0;}
