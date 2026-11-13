#include "spmp_fm03.h"
#include <stddef.h>
static void clear_bytes(void *target,size_t bytes){
    volatile unsigned char *p=(volatile unsigned char*)target;
    while(bytes--)*p++=0;
}
static unsigned safe_name(const char *p,unsigned max) {
    unsigned i=0;
    if(!p)return 0;
    while(i<max && p[i]){
        unsigned char c=(unsigned char)p[i];
        if(c=='/' || c=='\\' || c<32u || c==127u)return 0;
        i++;
    }
    if(!i || i==max || (i==1 && p[0]=='.') || (i==2 && p[0]=='.' && p[1]=='.'))return 0;
    return i;
}
static int valid_directory(const char *p){
    unsigned i=0,j=0;
    if(!p)return 0;
    if(!p[0])return 1;
    while(p[i] && i<FM01_PATH_MAX){
        unsigned char c=(unsigned char)p[i++];
        if(c=='/' ) {if(!j)return 0;j=0;continue;}
        if(c=='\\' || c<32u || c==127u)return 0;
        j++;
    }
    if(i==FM01_PATH_MAX || j==0)return 0;
    /* Do not permit dot/dotdot components */
    {
        unsigned s=0,k;
        for(k=0;k<=i;k++)if(k==i||p[k]=='/'){
            unsigned l=k-s;
            if((l==1&&p[s]=='.')||(l==2&&p[s]=='.'&&p[s+1]=='.'))return 0;
            s=k+1;
        }
    }
    return 1;
}
void fm03_init(fm03_model *m,fm01_model *browser,const fm03_backend *backend,void *ctx){
    if(!m)return;
    clear_bytes(m,sizeof(*m));
    m->browser=browser;m->ctx=ctx;
    if(backend)m->ops=*backend;
}
static void stop_scan(fm03_model *m){
    if(m && m->scan_started){if(m->ops.end)m->ops.end(m->ctx);m->scan_started=0;}
}
void fm03_close(fm03_model *m){
    if(!m)return;
    stop_scan(m);m->is_open=0;m->state=FM03_CLOSED;
}
int fm03_open(fm03_model *m,const char *directory,const char *name){
    int rc;
    if(!m || !m->ops.info || !m->ops.space || !m->ops.begin || !m->ops.step || !m->ops.end ||
       !valid_directory(directory) || !safe_name(name,FM01_NAME_MAX))return -1;
    if(m->browser && !m->browser->sd_present)return -2;
    fm03_close(m);
    clear_bytes(&m->info,sizeof(m->info));
    clear_bytes(&m->volume,sizeof(m->volume));
    clear_bytes(&m->scan,sizeof(m->scan));
    m->is_open=1;m->has_info=0;m->last_error=0;
    rc=m->ops.info(m->ctx,directory,name,&m->info);
    if(rc){m->last_error=rc;m->state=FM03_ERROR;return rc;}
    m->has_info=1;
    rc=m->ops.space(m->ctx,&m->volume);
    if(rc)m->volume.valid=0; /* Volume info failure does not hide valid file properties. */
    if(m->info.kind==FM01_ENTRY_FILE){m->state=FM03_FILE;return 0;}
    if(m->info.kind!=FM01_ENTRY_DIR){m->last_error=-3;m->state=FM03_ERROR;return -3;}
    rc=m->ops.begin(m->ctx,directory,name);
    if(rc){m->last_error=rc;m->state=FM03_ERROR;return rc;}
    m->scan_started=1;m->state=FM03_SCANNING;
    return 0;
}
int fm03_open_selected(fm03_model *m){
    const fm01_entry *e;
    if(!m||!m->browser)return -1;
    e=fm01_selected(m->browser,m->browser->active);
    if(!e)return -1;
    return fm03_open(m,m->browser->panels[m->browser->active].path,e->name);
}
int fm03_tick(fm03_model *m,uint32_t budget){
    int rc;
    if(!m || !m->is_open)return -1;
    if(m->state!=FM03_SCANNING)return 0;
    if(m->browser && !m->browser->sd_present){
        m->last_error=-2;m->state=FM03_ERROR;stop_scan(m);return -2;
    }
    if(!budget)budget=1;
    if(budget>FM03_SCAN_BATCH_MAX)budget=FM03_SCAN_BATCH_MAX;
    rc=m->ops.step(m->ctx,budget,&m->scan);
    if(rc<0){m->last_error=rc;m->state=FM03_ERROR;stop_scan(m);return rc;}
    if(!rc){m->state=FM03_FINISHED;stop_scan(m);}
    return rc;
}
void fm03_cancel_scan(fm03_model *m){
    if(m && m->is_open && m->state==FM03_SCANNING){stop_scan(m);m->state=FM03_CANCELLED;}
}
int fm03_buttons(fm03_model *m,uint32_t pressed){
    if(!m || !m->is_open)return 0;
    if(pressed&FM01_KEY_B){fm03_close(m);return 1;}
    if(pressed&FM01_KEY_START){fm03_cancel_scan(m);return 1;}
    return 1;
}

int fm03_dispatch(fm03_model *m,uint32_t pressed){
    if(!m)return 0;
    if(m->is_open)return fm03_buttons(m,pressed);
    if((pressed&(FM01_KEY_START|FM01_KEY_SELECT))==(FM01_KEY_START|FM01_KEY_SELECT)){
        (void)fm03_open_selected(m);
        return 1;
    }
    return 0;
}
