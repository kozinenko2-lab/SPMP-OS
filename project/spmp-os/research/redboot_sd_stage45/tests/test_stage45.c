#include "spmp_stage45_sd_poll.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef struct {
    uint32_t samples[16];uint32_t index;uint32_t size;
    uint32_t calls;uint32_t notify;uint32_t last_result;
    uint32_t fail_at;uint32_t cancel_at;
} sim;
static int sample(void *p,uint32_t *s){sim *a=(sim*)p; a->calls++;
    if(a->fail_at && a->calls>=a->fail_at)return 0;
    *s=a->samples[a->index<a->size?a->index:0];
    if(a->index<a->size)a->index++;
    return 1;
}
static int cancelled(void *p){sim*a=(sim*)p;return a->cancel_at && a->calls>=a->cancel_at;}
static void report(void*p,enum spmp45_result r,uint32_t polls,uint32_t status,uint32_t mask){
    sim*a=(sim*)p;(void)polls;(void)status;(void)mask;a->notify++;a->last_result=(uint32_t)r;
}
static spmp45_poll_io make(sim *s,uint32_t mask,uint32_t n){
    spmp45_poll_io io={s,sample,cancelled,report,mask,n};return io;
}
int main(void){sim s={0};spmp45_poll_io io;spmp45_poll_result r;unsigned long checks=0;
#define T(x) do{assert(x);checks++;}while(0)
    s.samples[0]=0;s.samples[1]=0;s.samples[2]=0x40;s.size=3;
    io=make(&s,0x40,20);r=spmp45_poll_status(&io);
    T(r.result==SPMP45_REACHED);T(r.polls==3);T(r.matched==1);T(s.notify==1);
    memset(&s,0,sizeof(s));s.size=1;io=make(&s,0x4,SPMP45_MAX_SAMPLES);
    r=spmp45_poll_status(&io);T(r.result==SPMP45_LIMIT);T(r.polls==SPMP45_MAX_SAMPLES);
    T(s.notify==1);T(s.calls==SPMP45_MAX_SAMPLES);
    memset(&s,0,sizeof(s));s.samples[0]=0x20;s.size=1;io=make(&s,0x4,10);
    r=spmp45_poll_status(&io);T(r.result==SPMP45_STATUS_ERROR);T(r.error_status==0x20);T(r.polls==1);
    memset(&s,0,sizeof(s));s.samples[0]=0x44;s.size=1;io=make(&s,0x40,10);
    r=spmp45_poll_status(&io);T(r.result==SPMP45_REACHED);T(r.polls==1);
    memset(&s,0,sizeof(s));s.fail_at=2;io=make(&s,0x40,10);
    r=spmp45_poll_status(&io);T(r.result==SPMP45_READ_ERROR);T(r.polls==1);T(s.notify==1);
    memset(&s,0,sizeof(s));s.cancel_at=2;io=make(&s,0x40,10);
    r=spmp45_poll_status(&io);T(r.result==SPMP45_CANCELLED);T(r.polls==2);
    memset(&s,0,sizeof(s));io=make(&s,0,10);
    r=spmp45_poll_status(&io);T(r.result==SPMP45_INVALID);T(s.calls==0);
    io=make(&s,0x4,SPMP45_MAX_SAMPLES+1);
    r=spmp45_poll_status(&io);T(r.result==SPMP45_INVALID);T(s.calls==0);
    for(uint32_t k=1;k<2000;k++){
        memset(&s,0,sizeof(s));s.size=1;io=make(&s,0x40,k);
        r=spmp45_poll_status(&io);T(r.result==SPMP45_LIMIT);T(r.polls==k);
    }
    printf("Stage45 read-only SD status sampler passed: %lu assertions\n",checks);
    return 0;
}
