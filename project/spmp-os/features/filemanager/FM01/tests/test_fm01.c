#include "spmp_fm01.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int mock_list(void *ctx,const char *path,uint32_t off,uint32_t cap,
    fm01_entry *out,uint32_t *n,uint32_t *total,uint32_t *files,uint32_t *dirs) {
    uint32_t i,cnt;
    (void)ctx;
    if(!strcmp(path,"")) {*total=277;*files=275;*dirs=2;}
    else if(!strcmp(path,"alpha")) {*total=2;*files=2;*dirs=0;}
    else if(!strcmp(path,"beta")) {*total=0;*files=0;*dirs=0;}
    else return -9;
    cnt=*total;
    *n=0;
    for(i=off;i<cnt && *n<cap;i++){
        fm01_entry *e=&out[*n];memset(e,0,sizeof(*e));
        if(!strcmp(path,"")){
            if(i==0){strcpy(e->name,"alpha");e->type=FM01_ENTRY_DIR;}
            else if(i==1){strcpy(e->name,"beta");e->type=FM01_ENTRY_DIR;}
            else {snprintf(e->name,sizeof(e->name),"file%03u.txt",i-2);e->type=FM01_ENTRY_FILE;e->size_bytes=2048+i;}
        }else{snprintf(e->name,sizeof(e->name),"nested%u.txt",i);e->type=FM01_ENTRY_FILE;}
        e->year=2026;e->month=11;e->day=11;
        (*n)++;
    }
    return 0;
}
static void fake_font(void *ctx,uint16_t *px,unsigned pitch,unsigned x,unsigned y,
    const char *s,uint16_t c,unsigned maxw,unsigned pixels) {
    unsigned i; (void)ctx;(void)s;(void)pixels;
    /* C renderer delegates glyphs to the system font; this probe only validates clipping. */
    assert(x<FM01_WIDTH && y<FM01_HEIGHT && maxw<=FM01_WIDTH);
    for(i=0;i<maxw && x+i<FM01_WIDTH;i+=4)px[y*pitch+x+i]=c;
}
int main(void){
    fm01_model *m=(fm01_model*)calloc(1,sizeof(*m));
    uint16_t *guard=(uint16_t*)malloc((FM01_WIDTH*FM01_HEIGHT+12)*sizeof(uint16_t));
    fm01_palette colors;fm01_font font={fake_font,0};
    fm01_status status={{'1','2',':','3','4',0},65,1};
    fm01_panel *p;fm01_entry e={0};char txt[64];unsigned i;
    assert(m && guard);
    fm01_init(m,mock_list,0);
    assert(m->panels[0].total==277 && m->panels[1].file_count==275);
    assert(m->panels[0].entries[0].type==FM01_ENTRY_DIR);
    assert(fm01_enter(m)==0);
    assert(!strcmp(m->panels[0].path,"alpha") && m->panels[0].total==2);
    assert(fm01_up(m)==0 && !strcmp(m->panels[0].path,""));
    assert(fm01_up(m)==1);
    for(i=0;i<280;i++)fm01_buttons(m,FM01_KEY_DOWN);
    p=&m->panels[0];
    assert(p->cursor==276 && p->first==270 && p->loaded==7);
    assert(!strcmp(fm01_selected(m,0)->name,"file274.txt"));
    fm01_buttons(m,FM01_KEY_RIGHT);
    assert(m->active==1 && m->panels[1].cursor==0);
    fm01_buttons(m,FM01_KEY_DOWN);
    assert(m->panels[1].cursor==1 && m->panels[0].cursor==276);
    fm01_buttons(m,FM01_KEY_A);
    assert(!strcmp(m->panels[1].path,"beta") && m->panels[1].total==0);
    fm01_buttons(m,FM01_KEY_B);
    assert(!strcmp(m->panels[1].path,"") && m->panels[1].cursor==1);
    fm01_buttons(m,FM01_KEY_SELECT);
    assert(m->active==0);
    strcpy(e.name,"Пример.log");e.year=2026;e.month=11;e.day=11;
    fm01_date_text(&e,txt,sizeof(txt));assert(!strcmp(txt,"11.11.26"));
    fm01_size_text(1024,txt,sizeof(txt));assert(!strcmp(txt,"1.0 KiB"));
    fm01_size_text(42,txt,sizeof(txt));assert(!strcmp(txt,"42 B"));
    fm01_palette_default(&colors);
    for(i=0;i<FM01_WIDTH*FM01_HEIGHT+12;i++)guard[i]=0xBEEF;
    fm01_render(m,guard+6,FM01_WIDTH,&colors,&font,&status);
    for(i=0;i<6;i++){assert(guard[i]==0xBEEF);assert(guard[FM01_WIDTH*FM01_HEIGHT+6+i]==0xBEEF);}
    status.sd_present=0;fm01_render(m,guard+6,FM01_WIDTH,&colors,&font,&status);
    for(i=0;i<6;i++){assert(guard[i]==0xBEEF);assert(guard[FM01_WIDTH*FM01_HEIGHT+6+i]==0xBEEF);}
    free(guard);free(m);
    puts("FM01 model/navigation/status/RGB565 bounds tests: PASS");
    return 0;
}
