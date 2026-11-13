#include "spmp_bootlog.h"
#include <stddef.h>

#define LOG_TEXT_LINE 3u
static void clear_bytes(void *dst, size_t n) {
    unsigned char *p=(unsigned char*)dst;
    while(n--) *p++=0;
}
static uint32_t checksum_record(const spmp_log_record *r) {
    /* FNV-1a for corruption detection, not cryptographic authenticity. */
    uint32_t h=2166136261u;
    const unsigned char *p=(const unsigned char*)r;
    size_t i;
    for(i=0;i<offsetof(spmp_log_record,checksum);i++) {
        h=(h^p[i])*16777619u;
    }
    return h;
}
int spmp_log_is_valid(const spmp_bootlog *l) {
    uint32_t i;
    if(!l || l->magic!=SPMP_LOG_MAGIC || l->version!=SPMP_LOG_VERSION ||
       l->head>=SPMP_LOG_RING_CAP || l->count>SPMP_LOG_RING_CAP ||
       l->line_len>=SPMP_LOG_LINE_BYTES || l->next_sequence==0) return 0;
    for(i=0;i<l->count;i++) {
        const spmp_log_record *r=&l->entries[(l->head+SPMP_LOG_RING_CAP-l->count+i)%SPMP_LOG_RING_CAP];
        if(r->msg_len>SPMP_LOG_MSG_BYTES || r->checksum!=checksum_record(r)) return 0;
    }
    return 1;
}
void spmp_log_init(spmp_bootlog *l, uint32_t session, int preserve) {
    if(!l) return;
    if(preserve && spmp_log_is_valid(l)) {
        l->session=session;
        return;
    }
    clear_bytes(l,sizeof(*l));
    l->magic=SPMP_LOG_MAGIC;
    l->version=SPMP_LOG_VERSION;
    l->session=session;
    l->next_sequence=1;
}
void spmp_log_event(spmp_bootlog *l, uint32_t tick, uint16_t phase,
                    uint16_t level, uint32_t event, uint32_t a, uint32_t b,
                    const char *message) {
    spmp_log_record *r;
    uint32_t i=0;
    if(!l || l->magic!=SPMP_LOG_MAGIC || l->head>=SPMP_LOG_RING_CAP ||
       l->count>SPMP_LOG_RING_CAP || l->next_sequence==0) return;
    r=&l->entries[l->head];
    clear_bytes(r,sizeof(*r));
    r->sequence=l->next_sequence++;
    if(l->next_sequence==0) l->next_sequence=1;
    r->tick=tick; r->phase=phase; r->level=level;
    r->event=event; r->arg0=a; r->arg1=b;
    if(message) {
        while(i<SPMP_LOG_MSG_BYTES && message[i]) {
            unsigned char c=(unsigned char)message[i];
            r->message[i]=((c>=0x20 && c<=0x7e) || (c&0x80))?(char)c:'?';
            i++;
        }
    }
    r->msg_len=(uint16_t)i;
    r->checksum=checksum_record(r);
    l->head=(l->head+1u)%SPMP_LOG_RING_CAP;
    if(l->count==SPMP_LOG_RING_CAP) l->dropped++;
    else l->count++;
}
void spmp_log_console_flush(spmp_bootlog *l, uint32_t tick) {
    if(!l || !l->line_len) return;
    l->pending_line[l->line_len]=0;
    spmp_log_event(l,tick,l->line_phase,SPMP_LOG_DEBUG,
                   LOG_TEXT_LINE,0,0,l->pending_line);
    l->line_len=0;
    l->pending_line[0]=0;
}
void spmp_log_console_char(spmp_bootlog *l,uint32_t tick,uint16_t phase,int c) {
    if(!l || l->magic!=SPMP_LOG_MAGIC) return;
    if(c=='\n' || c=='\r') {
        spmp_log_console_flush(l,tick);
        return;
    }
    if(c<0x20 && c!='\t') c='?';
    if(c=='\t') c=' ';
    if(l->line_len>=SPMP_LOG_LINE_BYTES-2u) spmp_log_console_flush(l,tick);
    if(!l->line_len) l->line_phase=phase;
    l->pending_line[l->line_len++]=(char)c;
    l->pending_line[l->line_len]=0;
}
static void addch(char *b, size_t *n, size_t max, char c) {
    if(*n<max) b[(*n)++]=c;
}
static void adds(char *b,size_t *n,size_t max,const char*s) {
    while(*s) addch(b,n,max,*s++);
}
static void hex32(char*b,size_t*n,size_t m,uint32_t v) {
    static const char h[]="0123456789abcdef";
    int i; for(i=7;i>=0;i--) addch(b,n,m,h[(v>>(i*4))&15u]);
}
static void dec32(char*b,size_t*n,size_t m,uint32_t v) {
    char rev[10]; unsigned k=0;
    do {rev[k++]=(char)('0'+v%10);v/=10;} while(v && k<10);
    while(k) addch(b,n,m,rev[--k]);
}
static int send(const spmp_log_sink *s,const char *b,size_t n) {
    return s->append(s->context,b,n)==(int)n;
}
int spmp_log_export_since(const spmp_bootlog*l,const spmp_log_sink*s,
                          uint32_t after, uint32_t max_records,
                          uint32_t *written, uint32_t *last_seq) {
    uint32_t i, done=0;
    char b[256]; size_t n;
    if(written) *written=0;
    if(last_seq) *last_seq=after;
    if(!spmp_log_is_valid(l) || !s || !s->append) return -1;
    n=0;
    adds(b,&n,sizeof(b),"SPMPTRACE v1 session="); hex32(b,&n,sizeof(b),l->session);
    adds(b,&n,sizeof(b)," count="); dec32(b,&n,sizeof(b),l->count);
    adds(b,&n,sizeof(b)," dropped="); dec32(b,&n,sizeof(b),l->dropped);
    addch(b,&n,sizeof(b),'\n');
    if(!send(s,b,n)) return -2;
    for(i=0;i<l->count && done<max_records;i++) {
        const spmp_log_record *r=&l->entries[(l->head+SPMP_LOG_RING_CAP-l->count+i)%SPMP_LOG_RING_CAP];
        uint32_t j;
        if(r->sequence<=after) continue;
        n=0;
        adds(b,&n,sizeof(b),"seq="); dec32(b,&n,sizeof(b),r->sequence);
        adds(b,&n,sizeof(b)," tick=0x"); hex32(b,&n,sizeof(b),r->tick);
        adds(b,&n,sizeof(b)," phase="); dec32(b,&n,sizeof(b),r->phase);
        adds(b,&n,sizeof(b)," lvl="); dec32(b,&n,sizeof(b),r->level);
        adds(b,&n,sizeof(b)," event=0x"); hex32(b,&n,sizeof(b),r->event);
        adds(b,&n,sizeof(b)," a=0x"); hex32(b,&n,sizeof(b),r->arg0);
        adds(b,&n,sizeof(b)," b=0x"); hex32(b,&n,sizeof(b),r->arg1);
        adds(b,&n,sizeof(b)," msg=");
        for(j=0;j<r->msg_len;j++) addch(b,&n,sizeof(b),r->message[j]);
        addch(b,&n,sizeof(b),'\n');
        if(!send(s,b,n)) return -2;
        done++;
        if(last_seq) *last_seq=r->sequence;
    }
    if(s->sync && s->sync(s->context)!=1) return -3;
    if(written) *written=done;
    return 0;
}

int spmp_log_export_text(const spmp_bootlog*l,const spmp_log_sink*s,
                         uint32_t max_records,uint32_t *written) {
    return spmp_log_export_since(l,s,0,max_records,written,0);
}
