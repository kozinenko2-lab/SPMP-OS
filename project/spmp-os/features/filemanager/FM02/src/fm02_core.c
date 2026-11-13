#include "spmp_fm02.h"
static size_t slen(const char *s,size_t limit) {size_t n=0;if(!s)return limit;while(n<limit && s[n])n++;return n;}
static int equal(const char *a,const char *b) {size_t i=0;for(;;i++){if(a[i]!=b[i])return 0;if(!a[i])return 1;}}
static void copy_string(char *dst,size_t size,const char *src) {size_t i=0;if(!size)return;while(i+1<size && src[i]){dst[i]=src[i];i++;}dst[i]=0;}
/* Strict UTF-8 basename: no path traversal, controls, surrogates, overlong encodings. */
int fm02_valid_name(const char *name) {
    size_t i=0,n=slen(name,FM01_NAME_MAX);if(!n||n==FM01_NAME_MAX)return 0;
    if((n==1 && name[0]=='.')||(n==2 && name[0]=='.'&&name[1]=='.'))return 0;
    while(i<n){unsigned c=(unsigned char)name[i++],v,min,len,j;
        if(c<0x80){if(c<32 || c==127 || c=='/' || c=='\\')return 0;continue;}
        if(c>=0xc2&&c<=0xdf){len=1;v=c&31;min=0x80;}
        else if(c>=0xe0&&c<=0xef){len=2;v=c&15;min=0x800;}
        else if(c>=0xf0&&c<=0xf4){len=3;v=c&7;min=0x10000;}
        else return 0;
        if(i+len>n)return 0;
        for(j=0;j<len;j++){c=(unsigned char)name[i++];if((c&0xc0)!=0x80)return 0;v=(v<<6)|(c&63);}
        if(v<min || v>0x10ffff || (v>=0xd800&&v<=0xdfff))return 0;
    }
    return 1;
}
void fm02_clear_marks(fm02_model *m) {if(m){m->marks_count=0;m->mark_directory[0]=0;}}
void fm02_init(fm02_model *m,fm01_model *browser,fm02_backend_action backend,void *ctx) {
    unsigned char *p=(unsigned char*)m;size_t i;if(!m)return;
    for(i=0;i<sizeof(*m);i++)p[i]=0;
    m->browser=browser;m->backend=backend;m->backend_context=ctx;m->state=FM02_IDLE;
}
static int mark_lookup(const fm02_model *m,const char *name){uint32_t i;for(i=0;i<m->marks_count;i++)if(equal(m->marks[i],name))return (int)i;return -1;}
static int marks_same_panel(const fm02_model *m) {
    return m->browser && m->browser->active==m->marks_panel &&
           equal(m->browser->panels[m->marks_panel].path,m->mark_directory);
}
int fm02_is_marked(const fm02_model *m,const char *dir,const char *name,unsigned which){
    if(!m||which>1||which!=m->marks_panel||!equal(dir,m->mark_directory))return 0;
    return mark_lookup(m,name)>=0;
}
int fm02_toggle_mark(fm02_model *m) {
    const fm01_entry *e;int index;uint32_t j;fm01_panel *p;
    if(!m||!m->browser||m->state==FM02_BUSY)return -1;
    p=&m->browser->panels[m->browser->active];e=fm01_selected(m->browser,m->browser->active);
    if(!e||!fm02_valid_name(e->name))return -2;
    if(m->marks_count && !marks_same_panel(m))fm02_clear_marks(m);
    if(!m->marks_count){m->marks_panel=m->browser->active;copy_string(m->mark_directory,sizeof(m->mark_directory),p->path);}
    index=mark_lookup(m,e->name);
    if(index>=0){for(j=(uint32_t)index;j+1<m->marks_count;j++)copy_string(m->marks[j],FM01_NAME_MAX,m->marks[j+1]);m->marks_count--;return 0;}
    if(m->marks_count==FM02_MARK_LIMIT)return -3;
    copy_string(m->marks[m->marks_count++],FM01_NAME_MAX,e->name);return 1;
}
int fm02_prepare(fm02_model *m,fm02_action action,const char *new_name) {
    const fm01_entry *e;
    if(!m||!m->browser||!m->backend||!m->browser->sd_present || m->state==FM02_BUSY)return -1;
    if(action<FM02_COPY||action>FM02_DELETE)return -2;
    if(m->marks_count && !marks_same_panel(m))fm02_clear_marks(m);
    e=fm01_selected(m->browser,m->browser->active);
    if(!m->marks_count && (!e||!fm02_valid_name(e->name)))return -3;
    if(action==FM02_RENAME && (m->marks_count>1||!fm02_valid_name(new_name)))return -4;
    if(action==FM02_COPY||action==FM02_MOVE){
        const char *s=m->browser->panels[m->browser->active].path;
        const char *d=m->browser->panels[!m->browser->active].path;
        if(equal(s,d))return -5;
    }
    m->last_action=action;m->last_error=0;m->processed=0;
    m->requested=m->marks_count?m->marks_count:1;
    m->confirmation=action==FM02_DELETE?1u:0u;
    m->state=action==FM02_DELETE?FM02_CONFIRM_DELETE:FM02_IDLE;
    if(action==FM02_RENAME)copy_string(m->rename_name,FM01_NAME_MAX,new_name);
    return 0;
}
static int progress_event(void *context,const fm02_progress *p) {
    fm02_model *m=(fm02_model*)context;
    m->progress=*p;
    if(m->poll_cancel && m->poll_cancel(m->poll_context))m->cancel_requested=1;
    return m->cancel_requested?1:0;
}
int fm02_commit(fm02_model *m,int confirmed) {
    fm01_model *b;fm02_request req;uint32_t i,n;int rc=0;
    if(!m||!m->browser||!m->backend||m->state==FM02_BUSY||!m->last_action)return -1;
    if(m->last_action==FM02_DELETE && !confirmed){m->state=FM02_CANCELLED;m->confirmation=0;return 1;}
    if(!m->browser->sd_present)return -2;
    b=m->browser;m->cancel_requested=0;m->confirmation=0;
    n=m->marks_count?m->marks_count:1;
    m->state=FM02_BUSY;m->progress.bytes_done=m->progress.bytes_total=0;
    m->progress.items_done=m->progress.items_total=0;m->progress.phase=0;
    for(i=0;i<n;i++) {
        const fm01_entry *selected=fm01_selected(b,b->active);
        if(!m->marks_count && !selected){rc=-3;break;}
        req.action=m->last_action;
        copy_string(req.src_directory,sizeof(req.src_directory),b->panels[b->active].path);
        copy_string(req.dst_directory,sizeof(req.dst_directory),b->panels[!b->active].path);
        copy_string(req.name,sizeof(req.name),m->marks_count?m->marks[i]:selected->name);
        req.new_name[0]=0;
        if(req.action==FM02_RENAME)copy_string(req.new_name,sizeof(req.new_name),m->rename_name);
        if(!fm02_valid_name(req.name)){rc=-4;break;}
        rc=m->backend(m->backend_context,&req,progress_event,m,&m->progress);
        if(rc){break;}m->processed++;
    }
    m->last_error=rc;m->state=rc?(m->cancel_requested?FM02_CANCELLED:FM02_FAILED):FM02_DONE;
    (void)fm01_refresh(b,0);(void)fm01_refresh(b,1);
    if(!rc)fm02_clear_marks(m);
    return rc;
}
void fm02_set_cancel_poll(fm02_model *m,fm02_poll_cancel poll,void *ctx){
    if(m){m->poll_cancel=poll;m->poll_context=ctx;}
}
void fm02_cancel(fm02_model *m){if(!m)return;m->cancel_requested=1;if(m->state==FM02_CONFIRM_DELETE){m->state=FM02_CANCELLED;m->confirmation=0;}}
int fm02_supply_rename(fm02_model *m,const char *name){
    if(!m||!fm02_valid_name(name))return -1;
    copy_string(m->rename_name,sizeof(m->rename_name),name);
    if(m->state==FM02_RENAME_INPUT_REQUIRED){
        int rc=fm02_prepare(m,FM02_RENAME,name);if(!rc)rc=fm02_commit(m,1);
        return rc;
    }
    return 0;
}
void fm02_buttons(fm02_model *m,uint32_t pressed){
    if(!m||!m->browser)return;
    if(m->state==FM02_BUSY){if(pressed&FM01_KEY_B)fm02_cancel(m);return;}
    if(m->state==FM02_CONFIRM_DELETE){if(pressed&FM01_KEY_A)(void)fm02_commit(m,1);else if(pressed&FM01_KEY_B)fm02_cancel(m);return;}
    if(m->state==FM02_RENAME_INPUT_REQUIRED){if(pressed&FM01_KEY_B)m->state=FM02_IDLE;return;}
    if(m->state==FM02_MENU){
        if(pressed&FM01_KEY_B){m->state=FM02_IDLE;return;}
        if(pressed&FM01_KEY_UP){m->menu_action=(uint8_t)((m->menu_action+2u)%4u);return;}
        if(pressed&FM01_KEY_DOWN){m->menu_action=(uint8_t)((m->menu_action+1u)%4u);return;}
        if(pressed&FM01_KEY_A){
            fm02_action action=(fm02_action)(m->menu_action+1u);
            if(action==FM02_RENAME){m->state=FM02_RENAME_INPUT_REQUIRED;return;}
            if(fm02_prepare(m,action,0)==0 && action!=FM02_DELETE)(void)fm02_commit(m,1);
            return;
        }
        return;
    }
    if(pressed&FM01_KEY_START){m->menu_action=0;m->state=FM02_MENU;return;}
    if(pressed & FM01_KEY_SELECT){(void)fm02_toggle_mark(m);return;}
    if(pressed & (FM01_KEY_UP|FM01_KEY_DOWN|FM01_KEY_LEFT|FM01_KEY_RIGHT|FM01_KEY_A|FM01_KEY_B))fm01_buttons(m->browser,pressed);
}
