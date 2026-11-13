#include "probe.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct host {
    uint16_t pixels[2][320*240];
    unsigned front,frames,polls,finishes;
    uint16_t color,width,height;
    const uint32_t *script;
    size_t script_count;
    int fail_present;
};
static struct host host;
static uint32_t begin(void *p){struct host *h=p;h->front=0;return 0;}
static uint32_t dimensions(void *p,uint16_t *w,uint16_t *h){struct host *s=p;*w=s->width;*h=s->height;return 0;}
static uint32_t get_color(void *p,struct mmi_rgb888 *rgb){uint16_t v=((struct host *)p)->color;rgb->r=(v>>8)&248;rgb->g=(v>>3)&252;rgb->b=(v<<3)&248;return 0;}
static uint32_t set_color(void *p,const struct mmi_rgb888 *rgb){((struct host *)p)->color=(uint16_t)(((rgb->r&248)<<8)|((rgb->g&252)<<3)|(rgb->b>>3));return 0;}
static uint32_t fill(void *p,const struct probe_rect *r)
{
    struct host *h=p;
    if(r->x<0||r->y<0||(unsigned)r->x+r->width>320||(unsigned)r->y+r->height>240)return 1;
    for(unsigned y=(unsigned)r->y;y<(unsigned)r->y+r->height;y++)
        for(unsigned x=(unsigned)r->x;x<(unsigned)r->x+r->width;x++)h->pixels[h->front^1][y*320+x]=h->color;
    return 0;
}
static uint32_t present(void *p)
{
    struct host *h=p;if(h->fail_present)return 1;
    h->front^=1;memcpy(h->pixels[h->front^1],h->pixels[h->front],sizeof h->pixels[0]);h->frames++;return 0;
}
static uint32_t keys(void *p,struct mmi_key_output32 *out)
{
    struct host *h=p;out->keys=h->polls<h->script_count?h->script[h->polls]:0;h->polls++;return 1;
}
static void finish(void *p){((struct host *)p)->finishes++;}
static const struct probe_ops ops={begin,dimensions,get_color,set_color,fill,present,keys,finish};
static const struct probe_config config={64,1,2,4,8,0x20000};
static void reset(const uint32_t *script,size_t n){memset(&host,0,sizeof host);host.width=320;host.height=240;host.color=0x1234;host.script=script;host.script_count=n;}
static int frame(const char *path)
{
    FILE *f=fopen(path,"wb");if(!f)return 0;
    int ok=fprintf(f,"P6\n320 240\n255\n")>0;
    for(size_t i=0;i<320*240&&ok;i++){
        uint16_t v=host.pixels[host.front][i];unsigned char rgb[3]={(unsigned char)((v>>8)&248),(unsigned char)((v>>3)&252),(unsigned char)((v<<3)&248)};
        ok=fwrite(rgb,1,3,f)==3;
    }
    return fclose(f)==0&&ok;
}
static void tests(void)
{
    struct probe_state s;
    const uint32_t quit[]={0x20000};reset(quit,1);
    assert(probe_start(&s,&ops,&host,&config)==PROBE_RUNNING);
    assert(probe_step(&s)==PROBE_RETURN&&host.frames==0&&host.finishes==1);
    assert(probe_step(&s)==PROBE_RETURN&&host.polls==1&&host.finishes==1);
    reset(NULL,0);struct probe_config limited=config;limited.max_steps=2;
    assert(probe_start(&s,&ops,&host,&limited)==PROBE_RUNNING);
    assert(probe_step(&s)==PROBE_RUNNING&&probe_step(&s)==PROBE_RETURN);
    assert(host.frames==2&&host.polls==2&&host.finishes==1&&host.color==0x1234);
    reset(NULL,0);host.width=640;
    assert(probe_start(&s,&ops,&host,&config)==PROBE_ERROR&&host.finishes==1&&host.polls==0);
    reset(NULL,0);host.fail_present=1;
    assert(probe_start(&s,&ops,&host,&config)==PROBE_RUNNING);
    assert(probe_step(&s)==PROBE_ERROR&&host.finishes==1&&host.color==0x1234);
    reset(NULL,0);limited.max_steps=0;
    assert(probe_start(&s,&ops,&host,&limited)==PROBE_ERROR&&host.finishes==0);
}
int main(int argc,char **argv)
{
    const char *output=NULL;
    if(argc==3&&strcmp(argv[1],"--frame")==0)output=argv[2];
    else if(argc!=1){fprintf(stderr,"usage: %s [--frame output.ppm]\n",argv[0]);return 2;}
    tests();
    const uint32_t script[]={8,8,2,2,0x20000};reset(script,sizeof script/sizeof script[0]);
    struct probe_state state;enum probe_result result=probe_start(&state,&ops,&host,&config);
    while(result==PROBE_RUNNING)result=probe_step(&state);
    assert(result==PROBE_RETURN&&host.frames==4&&host.finishes==1&&state.x==164&&state.y==124);
    assert(host.pixels[host.front][0]==0xf800&&host.pixels[host.front][150]==0x07e0&&host.pixels[host.front][319]==0x001f);
    assert(host.pixels[host.front][124*320+164]==0xffff);
    if(output&&!frame(output)){fprintf(stderr,"cannot write host frame\n");return 1;}
    puts("PASS: host probe draws RGB565 bars/cursor, polls scripted masks, bounds steps, restores color and requests normal return");
    return 0;
}
