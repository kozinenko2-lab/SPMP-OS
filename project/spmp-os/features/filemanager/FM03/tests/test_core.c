/* Deterministic model tests: no POSIX, no UI, no allocation. */
#include "spmp_fm03.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static unsigned calls,begins,ends,budget_seen;
static int info(void *ctx,const char *dir,const char *name,fm03_info *out){
    (void)ctx;(void)dir;
    if(!strcmp(name,"missing"))return -11;
    memset(out,0,sizeof(*out));
    out->kind=!strcmp(name,"folder")?FM01_ENTRY_DIR:FM01_ENTRY_FILE;
    out->bytes=1250;out->year=2026;out->month=11;out->day=11;
    memcpy(out->name,name,strlen(name)+1);
    memcpy(out->path,name,strlen(name)+1);
    return 0;
}
static int space(void *ctx,fm03_volume *out){(void)ctx;*out=(fm03_volume){1000,400,300,1};return 0;}
static int begin(void *ctx,const char *d,const char *n){(void)ctx;(void)d;(void)n;begins++;return 0;}
static int step(void *ctx,uint32_t budget,fm03_scan *out){
    (void)ctx;budget_seen=budget;calls++;
    out->files+=budget;out->bytes+=budget*50u;
    return calls>=3?0:1;
}
static void end(void *ctx){(void)ctx;ends++;}
int main(void){
    fm03_backend b={info,space,begin,step,end};fm03_model m;
    fm03_init(&m,NULL,&b,NULL);
    assert(fm03_open(&m,"","file.txt")==0 && m.state==FM03_FILE && m.has_info);
    assert(m.info.bytes==1250 && m.volume.valid && begins==0);
    assert(fm03_buttons(&m,FM01_KEY_B)==1 && !m.is_open);
    assert(fm03_open(&m,"","folder")==0 && m.state==FM03_SCANNING && begins==1);
    assert(fm03_tick(&m,1000)==1 && budget_seen==64 && m.scan.files==64);
    assert(fm03_tick(&m,1)==1 && m.scan.files==65);
    assert(fm03_tick(&m,2)==0 && m.state==FM03_FINISHED && ends==1);
    assert(fm03_tick(&m,1)==0 && ends==1);
    assert(fm03_open(&m,"","folder")==0 && m.state==FM03_SCANNING);
    fm03_cancel_scan(&m);assert(m.state==FM03_CANCELLED&&ends==2);
    fm03_close(&m);assert(ends==2);
    assert(fm03_open(&m,"../","file.txt")<0);
    assert(fm03_open(&m,"a//b","file.txt")<0);
    assert(fm03_open(&m,"/etc","file.txt")<0);
    assert(fm03_open(&m,"","..")<0);
    assert(fm03_open(&m,"","missing")<0 && m.state==FM03_ERROR);
    {
        fm01_model browser={0};
        fm03_init(&m,&browser,&b,NULL);
        browser.sd_present=0;
        assert(fm03_open(&m,"","file.txt")==-2);
        assert(fm03_dispatch(&m,FM01_KEY_START)==0);
        browser.sd_present=1;
        browser.panels[0].loaded=1;
        strcpy(browser.panels[0].entries[0].name,"file.txt");
        assert(fm03_dispatch(&m,FM01_KEY_START|FM01_KEY_SELECT)==1);
        assert(m.state==FM03_FILE && m.is_open);
        assert(fm03_dispatch(&m,FM01_KEY_B)==1 && !m.is_open);
    }
    puts("FM03 model: OK");
    return 0;
}
