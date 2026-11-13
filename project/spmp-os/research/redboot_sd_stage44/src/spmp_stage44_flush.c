#include "spmp_stage44_flush.h"
typedef struct {const spmp_stage44_fs *fs;} sinkctx;
static int send_data(void *opaque,const char *data,size_t len) {
    sinkctx *p=(sinkctx*)opaque;
    return p->fs->write(p->fs->context,data,len);
}
static void path8(char out[25],uint32_t session){
    static const char pre[]="SPMPOS/LOGS/";
    static const char hx[]="0123456789ABCDEF";
    for(unsigned i=0;i<12;i++)out[i]=pre[i];
    for(unsigned i=0;i<8;i++)out[12+i]=hx[(session>>(28u-4u*i))&15u];
    out[20]='.';out[21]='L';out[22]='O';out[23]='G';out[24]='\0';
}
int spmp_stage44_flush(const spmp_bootlog *log,
    spmp_stage44_flush_state *state,const spmp_stage44_fs *fs,
    uint32_t max_records){
    uint32_t after,last,written=0;
    char path[25];sinkctx ctx;spmp_log_sink sink;
    int exported,closed;
    if(!state || !fs || !log || !fs->ready || !fs->open_append ||
       !fs->write || !fs->close_checked || max_records==0 ||
       !spmp_log_is_valid(log) || state->active)return -1;
    if(fs->ready(fs->context)!=1)return 0;
    /* Reset cursor when session changes (session uniqueness is caller duty). */
    if(state->session!=log->session){
        state->session=log->session;state->acknowledged_sequence=0;
    }
    if(!log->count)return 0;
    if(log->entries[(log->head+SPMP_LOG_RING_CAP-1u)%SPMP_LOG_RING_CAP]
       .sequence<=state->acknowledged_sequence)return 0;
    if(max_records>32u)max_records=32u; /* latency and wear bound */
    state->active=1;
    path8(path,log->session);
    if(fs->open_append(fs->context,path)!=1){
        state->io_failures++;state->active=0;return -1;
    }
    ctx.fs=fs;
    sink.context=&ctx;sink.append=send_data;sink.sync=0;
    after=state->acknowledged_sequence;last=after;
    exported=spmp_log_export_since(log,&sink,after,max_records,&written,&last);
    closed=fs->close_checked(fs->context);
    if(exported!=0 || closed!=1){
        state->io_failures++;state->active=0;return -1;
    }
    if(written){state->acknowledged_sequence=last;state->successful_closes++;}
    state->active=0;
    return written?1:0;
}
