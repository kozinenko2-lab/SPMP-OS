#include "spmp_stage40_file_ui.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {int reads;int readonly;int invalid;} mock;
static int confirmed(void *ctx){return ((mock*)ctx)->readonly;}
static int read_dir(void *ctx,const char *path,size_t skip,spmp_ro_entry *out,
                    size_t cap,size_t *written,size_t *total,size_t *files){
    mock *m=(mock*)ctx; m->reads++;
    if(strcmp(path,"/") && strcmp(path,"/games") && strcmp(path,"/docs")) return -1;
    size_t n=strcmp(path,"/games")==0?70:strcmp(path,"/")==0?2:1;
    *total=n; *files=strcmp(path,"/games")==0?70:0;
    *written=0;
    if(skip>n)return -1;
    for(size_t i=skip;i<n && *written<cap;i++){
        spmp_ro_entry *e=&out[(*written)++];memset(e,0,sizeof *e);
        if(!strcmp(path,"/")){
            snprintf(e->name,sizeof e->name,"%s",i==0?"games":"docs");e->is_dir=1;
        }else if(!strcmp(path,"/docs")){
            snprintf(e->name,sizeof e->name,"README.txt");
        }else{
            snprintf(e->name,sizeof e->name,"Игра_%02zu.gba",i);
        }
        e->bytes=(uint64_t)i*100000u+123u;
        e->fat_date=(uint16_t)(((2026u-1980u)<<9)|(11u<<5)|13u);
        if(m->invalid && i==0){strcpy(e->name,"..");}
    }
    return 0;
}
static int font_called=0, font_unicode=0;
static void fake_font(void *ctx,const char *s,uint32_t max_px,spmp40_plot plot,void *plot_ctx){
    (void)ctx;font_called++;
    if(strstr(s,"И"))font_unicode=1;
    for(uint32_t i=0;i<max_px+20u;i++)plot(plot_ctx,i,(i%15)); /* deliberate overdraw: must clip */
}
static spmp_ro_dual_panel dual;
static spmp_ro_panel scratch;
static uint8_t framebuffer[240*672+32];
int main(void){
    mock m={0,1,0};spmp_ro_fs fs={&m,confirmed,read_dir};
    spmp40_file_ui ui={0};
    assert(spmp40_init(&ui,&dual,&fs,&scratch,"/","/games")==0);
    assert(ui.dual==&dual && !strcmp(dual.panels[0].path,"/"));
    assert(spmp40_navigate(&ui,SPMP40_NAV_ENTER)==0);
    assert(!strcmp(dual.panels[0].path,"/games"));
    for(int k=0;k<70;k++)assert(spmp40_navigate(&ui,SPMP40_NAV_DOWN)==0);
    assert(dual.panels[0].page_offset==64 && dual.panels[0].selected==5);
    assert(spmp40_navigate(&ui,SPMP40_NAV_UP)==0);
    for(int k=0;k<5;k++)assert(spmp40_navigate(&ui,SPMP40_NAV_UP)==0);
    assert(dual.panels[0].page_offset==0 && dual.panels[0].selected==63);
    assert(spmp40_navigate(&ui,SPMP40_NAV_PARENT)==0);
    assert(!strcmp(dual.panels[0].path,"/"));
    assert(spmp40_navigate(&ui,SPMP40_NAV_PARENT)==0);
    assert(spmp40_navigate(&ui,SPMP40_NAV_SWITCH)==0 && dual.active==1);
    assert(spmp40_navigate(&ui,SPMP40_NAV_PAGE_DOWN)==0 && dual.panels[1].page_offset==64);
    assert(spmp40_navigate(&ui,SPMP40_NAV_PAGE_UP)==0 && dual.panels[1].page_offset==0);
    assert(spmp40_navigate(&ui,SPMP40_NAV_ENTER)==-1); // file, not directory
    assert(spmp40_navigate(&ui,(spmp40_nav)888)==-1);

    memset(framebuffer,0xA5,sizeof framebuffer);
    spmp39_surface16 surface={framebuffer,320,240,672,240*672};
    spmp40_pixel16 p={0x23,0x61};
    spmp40_theme theme={p,p,p,p,p,p,p};
    assert(spmp40_render(&ui,&surface,&theme,fake_font,0)==0);
    assert(font_called>10);
    assert(font_unicode==1);
    for(int y=0;y<240;y++)for(int x=640;x<672;x++)assert(framebuffer[y*672+x]==0xA5);
    for(size_t i=240u*672u;i<sizeof framebuffer;i++)assert(framebuffer[i]==0xA5);
    surface.capacity_bytes=240*672-1;
    assert(spmp40_render(&ui,&surface,&theme,fake_font,0)==-1);
    surface.capacity_bytes=240*672;
    surface.width=319;
    assert(spmp40_render(&ui,&surface,&theme,fake_font,0)==-1);
    surface.width=320;

    /* Hostile filesystem response rejected without altering current panel. */
    m.invalid=1;
    char path_before[SPMP_FS_PATH_MAX];strcpy(path_before,dual.panels[1].path);
    assert(spmp40_navigate(&ui,SPMP40_NAV_PARENT)==-1);
    assert(!strcmp(path_before,dual.panels[1].path));
    m.readonly=0; m.invalid=0;
    assert(spmp40_navigate(&ui,SPMP40_NAV_PAGE_DOWN)==-1);
    assert(spmp40_init(&ui,&dual,&fs,&scratch,"/","/games")==-1);
    m.readonly=1;
    assert(spmp40_init(&ui,&dual,&fs,&scratch,"/","/games")==0);
    printf("stage40: UI navigation, RO boundary, paging, guard bytes, font hook PASS (%d reads)\n",m.reads);
    return 0;
}
