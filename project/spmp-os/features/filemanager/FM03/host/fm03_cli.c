#define _POSIX_C_SOURCE 200809L
#include "fm03_posix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
    fm03_posix fs;fm03_backend backend;fm03_model model;
    int rc,rounds=0,cancel_after=0;
    if(argc<4||argc>5){fprintf(stderr,"Usage: fm03_props ROOT PARENT NAME [cancel-after-N-ticks]\n");return 2;}
    if(argc==5)cancel_after=atoi(argv[4]);
    rc=fm03_posix_init(&fs,argv[1]);if(rc){fprintf(stderr,"root error %d\n",rc);return 3;}
    fm03_posix_backend(&backend);fm03_init(&model,NULL,&backend,&fs);
    rc=fm03_open(&model,argv[2],argv[3]);
    while(rc>=0&&model.state==FM03_SCANNING){
        if(cancel_after && rounds>=cancel_after){fm03_cancel_scan(&model);break;}
        rc=fm03_tick(&model,8);rounds++;
    }
    printf("status=%d\nerror=%d\nname=%s\npath=%s\ntype=%u\nsize=%llu\n",
           model.state,model.last_error,model.info.name,model.info.path,
           (unsigned)model.info.kind,(unsigned long long)model.info.bytes);
    printf("modified=%02u.%02u.%02u %02u:%02u\n",model.info.day,model.info.month,
           model.info.year%100u,model.info.hour,model.info.minute);
    printf("files=%u\ndirs=%u\nbytes=%llu\nvisited=%u\nskipped=%u\nticks=%d\n",
           model.scan.files,model.scan.directories,(unsigned long long)model.scan.bytes,
           model.scan.visited,model.scan.skipped,rounds);
    printf("sd_valid=%u\nsd_total=%llu\nsd_available=%llu\n",model.volume.valid,
           (unsigned long long)model.volume.total_bytes,(unsigned long long)model.volume.available_bytes);
    fm03_close(&model);fm03_posix_close(&fs);
    return rc<0?1:0;
}
