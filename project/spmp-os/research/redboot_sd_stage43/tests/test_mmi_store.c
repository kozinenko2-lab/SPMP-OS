#include "spmp_mmi_trace_store.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct mock_fs {
  int ready, opened, closes, synced, fail_sync, fail_write;
  char path[64];char content[65536];size_t len;
};
static int ready(void*c){return ((struct mock_fs*)c)->ready;}
static int open_append(void*c,const char*path){
 struct mock_fs*f=c;f->opened++;strncpy(f->path,path,sizeof(f->path)-1);return 1;
}
static int write_cb(void*c,const char*b,size_t n){
 struct mock_fs*f=c;
 if(f->fail_write || f->len+n>sizeof(f->content))return -1;
 memcpy(f->content+f->len,b,n);f->len+=n;return (int)n;
}
static int sync_cb(void*c){struct mock_fs*f=c;f->synced++;return !f->fail_sync;}
static void close_cb(void*c){((struct mock_fs*)c)->closes++;}
static void test(void){
 static spmp_bootlog log;
 struct mock_fs f={0};
 spmp_mmi_filesystem cb={&f,ready,open_append,write_cb,sync_cb,close_cb};
 spmp_mmi_trace_state st={0};
 spmp_log_init(&log,0x12345678,0);
 spmp_log_event(&log,10,SPMP_LOG_RED_BOOT,SPMP_LOG_INFO,1,7,8,"redboot msg");
 assert(spmp_mmi_trace_flush(&log,&st,&cb)==0 && f.opened==0);
 f.ready=1;
 assert(spmp_mmi_trace_flush(&log,&st,&cb)==1);
 assert(!strcmp(f.path,"SPMPOS/LOGS/12345678.LOG"));
 assert(f.opened==1 && f.synced==1 && f.closes==1 && st.committed_sequence==1);
 size_t previous=f.len;
 assert(spmp_mmi_trace_flush(&log,&st,&cb)==0 && f.len==previous);
 spmp_log_event(&log,11,SPMP_LOG_MMI_BOOT,SPMP_LOG_INFO,2,0,0,"mmi entry");
 f.fail_sync=1;
 assert(spmp_mmi_trace_flush(&log,&st,&cb)==-1 && st.committed_sequence==1);
 f.fail_sync=0;
 assert(spmp_mmi_trace_flush(&log,&st,&cb)==1 && st.committed_sequence==2);
 assert(st.write_failures==1 && st.successful_flushes==2);
 spmp_log_event(&log,12,SPMP_LOG_MMI_FS,SPMP_LOG_ERROR,3,0,0,"mount issue");
 f.fail_write=1;
 assert(spmp_mmi_trace_flush(&log,&st,&cb)==-1 && st.committed_sequence==2);
 f.fail_write=0;
 assert(spmp_mmi_trace_flush(&log,&st,&cb)==1 && st.committed_sequence==3);
 printf("OK: post-mount write/no-SD/partial-failure/sync/retry/8.3 path\n");
}
int main(void){test();return 0;}
