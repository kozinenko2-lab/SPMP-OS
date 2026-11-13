#include "spmp_mmi_trace_store.h"

typedef struct {
    const spmp_mmi_filesystem *fs;
} file_context;
static int append(void*context,const char*text,size_t n) {
    file_context *f=(file_context*)context;
    return f->fs->write(f->fs->context,text,n);
}
static int sync_fn(void*context) {
    file_context *f=(file_context*)context;
    return f->fs->sync(f->fs->context);
}
/* Simpler robust fixed 8.3 name formatter. */
static void log_path(char out[26],uint32_t session) {
    static const char prefix[]="SPMPOS/LOGS/";
    static const char h[]="0123456789ABCDEF";
    unsigned i;
    for(i=0;i<12;i++)out[i]=prefix[i];
    for(i=0;i<8;i++)out[12+i]=h[(session>>(4u*(7u-i)))&15u];
    out[20]='.';out[21]='L';out[22]='O';out[23]='G';out[24]=0;
}
int spmp_mmi_trace_flush(const spmp_bootlog *l,
                         spmp_mmi_trace_state *state,
                         const spmp_mmi_filesystem *fs) {
    char path[26]; file_context fctx; spmp_log_sink sink;
    uint32_t last, written=0;
    int ret;
    if(!state || !fs || !fs->fs_ready_writable || !fs->open_append ||
       !fs->write || !fs->sync || !fs->close || !spmp_log_is_valid(l))return -1;
    if(fs->fs_ready_writable(fs->context)!=1)return 0;
    if(state->session!=l->session) {
        state->session=l->session;
        state->committed_sequence=0;
    }
    if(l->count==0)return 0;
    if(l->entries[(l->head+SPMP_LOG_RING_CAP-1u)%SPMP_LOG_RING_CAP].sequence<=state->committed_sequence)return 0;
    log_path(path,l->session);
    if(fs->open_append(fs->context,path)!=1) {
        state->write_failures++;
        return -1;
    }
    fctx.fs=fs;
    sink.context=&fctx;sink.append=append;sink.sync=sync_fn;
    last=state->committed_sequence;
    ret=spmp_log_export_since(l,&sink,state->committed_sequence,
                              SPMP_LOG_RING_CAP,&written,&last);
    fs->close(fs->context);
    if(ret!=0) {
        state->write_failures++;
        return -1;
    }
    if(written) {
        if(state->committed_sequence && last>state->committed_sequence+1u && written==1u) {
            state->pending_loss_count++;
        }
        state->committed_sequence=last;
        state->successful_flushes++;
    }
    return written?1:0;
}
