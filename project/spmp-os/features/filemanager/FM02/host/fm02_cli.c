/* SPDX-License-Identifier: MIT */
#include "fm02_posix.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
typedef struct {uint64_t cancel_at;unsigned notifications;int abort_scan;} controller;
static int progress(void *ctx,const fm02_progress *p){
    controller *c=(controller*)ctx;c->notifications++;
    return (c->abort_scan && p->phase==1) || (c->cancel_at && p->phase==2 && p->bytes_done>=c->cancel_at);
}
int main(int argc,char **argv){
    fm02_posix fs;fm02_request req;fm02_progress result;controller ctrl={0,0,0};int rc;
    if(argc<5 || argc>7){
        fprintf(stderr,"usage: %s ROOT copy|move|rename|delete SRC_DIR NAME [DST_DIR|NEW_NAME] [cancel_bytes|scan]\n",argv[0]);return 64;
    }
    memset(&req,0,sizeof(req));memset(&result,0,sizeof(result));
    if(!strcmp(argv[2],"copy"))req.action=FM02_COPY;
    else if(!strcmp(argv[2],"move"))req.action=FM02_MOVE;
    else if(!strcmp(argv[2],"rename"))req.action=FM02_RENAME;
    else if(!strcmp(argv[2],"delete"))req.action=FM02_DELETE;
    else return 64;
    if(strlen(argv[3])>=sizeof(req.src_directory)||strlen(argv[4])>=sizeof(req.name))return 64;
    strcpy(req.src_directory,argv[3]);strcpy(req.name,argv[4]);
    if(req.action!=FM02_DELETE){
        if(argc<6)return 64;
        if(req.action==FM02_RENAME){if(strlen(argv[5])>=sizeof(req.new_name))return 64;strcpy(req.new_name,argv[5]);}
        else {if(strlen(argv[5])>=sizeof(req.dst_directory))return 64;strcpy(req.dst_directory,argv[5]);}
    }
    if(argc>=6 && req.action==FM02_DELETE){if(!strcmp(argv[5],"scan"))ctrl.abort_scan=1;else ctrl.cancel_at=strtoull(argv[5],0,10);}
    if(argc>=7 && req.action!=FM02_DELETE){if(!strcmp(argv[6],"scan"))ctrl.abort_scan=1;else ctrl.cancel_at=strtoull(argv[6],0,10);}
    fs.rootfd=-1;rc=fm02_posix_init(&fs,argv[1]);if(rc){fprintf(stderr,"init error %d\n",rc);return 2;}
    rc=fm02_posix_action(&fs,&req,progress,&ctrl,&result);
    printf("result=%d bytes=%" PRIu64 "/%" PRIu64 " items=%u/%u phase=%u callbacks=%u\n",rc,
           result.bytes_done,result.bytes_total,result.items_done,result.items_total,
           result.phase,ctrl.notifications);
    fm02_posix_close(&fs);
    return rc?1:0;
}
