#include "spmp_mmi49_logger.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct {
    int ready, open_result, close_result, short_write;
    int opens, writes, closes;
    char file[32768]; size_t used;
} mock_fs;
static int ready(void *p){return ((mock_fs*)p)->ready;}
static int open_file(void *p,const char *path){mock_fs*m=p;(void)path;m->opens++;return m->open_result;}
static int write_file(void *p,const char *data,size_t n){mock_fs*m=p;size_t k=n;
    m->writes++;
    if(m->short_write && k)k--;
    assert(m->used+k<=sizeof(m->file));
    memcpy(m->file+m->used,data,k);m->used+=k;return (int)k;
}
static int close_file(void *p){mock_fs*m=p;m->closes++;return m->close_result;}
static spmp_mmi49_fs adapter(mock_fs*m){spmp_mmi49_fs f={m,ready,open_file,write_file,close_file};return f;}
static void original_putchar(void*p,int c){int*counter=p;*counter+=c==65?1:0;}
static void reset_mock(mock_fs*m){memset(m,0,sizeof(*m));m->ready=1;m->open_result=1;m->close_result=1;}
int main(void){spmp_mmi49_logger l;mock_fs m;spmp_mmi49_fs f;int i;
    spmp_mmi49_init(&l,0);spmp_mmi49_capture_text(&l,"X");assert(l.used==0);
    spmp_mmi49_init(&l,1);spmp_mmi49_capture_text(&l,"Hello\n");
    spmp_mmi49_event(&l,1,"FAT_MOUNT",0);assert(l.used>6);
    int x=0;spmp_mmi49_forward(&l,original_putchar,&x,65);assert(x==1);
    reset_mock(&m);f=adapter(&m);
    m.ready=0;assert(spmp_mmi49_flush(&l,&f,"/fat20a0/hda1/MMI_BOOT.LOG",512)==0);assert(m.opens==0);
    m.ready=1;assert(spmp_mmi49_flush(&l,&f,"/Rom/image/8000_mmi.rap",512)==-1);assert(m.opens==0);
    assert(spmp_mmi49_flush(&l,&f,"../MMI_BOOT.LOG",512)==-1);
    assert(spmp_mmi49_flush(&l,&f,"/fat20a0/hda1/../BOGUS",512)==-1);
    assert(spmp_mmi49_flush(&l,&f,"/fat20a0/hda1/MMI_BOOT.LOG",512)==1);
    assert(l.acknowledged==l.used && m.closes==1);
    assert(spmp_mmi49_flush(&l,&f,"/fat20a0/hda1/MMI_BOOT.LOG",512)==0);
    spmp_mmi49_capture_text(&l,"failed-after-write");
    m.close_result=0;size_t last=l.acknowledged;
    assert(spmp_mmi49_flush(&l,&f,"/fat20a0/hda1/MMI_BOOT.LOG",512)==-1);
    assert(l.acknowledged==last);m.close_result=1;
    assert(spmp_mmi49_flush(&l,&f,"/fat20a0/hda1/MMI_BOOT.LOG",512)==1);
    spmp_mmi49_capture_text(&l,"partial-write");m.short_write=1;
    last=l.acknowledged;
    assert(spmp_mmi49_flush(&l,&f,"/fat20a0/hda1/MMI_BOOT.LOG",512)==-1);
    assert(l.acknowledged==last);
    /* No realtime console or SD calls from capture; 10k deterministic char taps. */
    spmp_mmi49_init(&l,1);reset_mock(&m);f=adapter(&m);
    for(i=0;i<10000;i++)spmp_mmi49_capture_char(&l,'a'+i%26);
    assert(l.used==SPMP_MMI49_CAPACITY);assert(l.dropped==10000-SPMP_MMI49_CAPACITY);
    for(i=0;i<100;i++) {
        int r=spmp_mmi49_flush(&l,&f,"/fat20a0/hda1/MMI_BOOT.LOG",1000);
        assert(r==1 || r==0);
        if(r==0)break;
    }
    assert(l.acknowledged==SPMP_MMI49_CAPACITY);
    assert(m.used==SPMP_MMI49_CAPACITY);
    for(i=0;i<(int)SPMP_MMI49_CAPACITY;i++)assert(m.file[i]=='a'+i%26);
    puts("Stage49: all MMI buffer/SD-gating/partial IO/close/error/8192B tests passed");return 0;
}
