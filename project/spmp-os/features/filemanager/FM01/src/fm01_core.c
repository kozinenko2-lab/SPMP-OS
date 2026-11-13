#include "spmp_fm01.h"

static void bytes_copy(char *out,const char *in,size_t max) {
    size_t i=0;
    if (!max) return;
    while (i+1<max && in[i]) { out[i]=in[i]; i++; }
    out[i]=0;
}
static size_t bytes_len(const char *s) {size_t n=0;while(s[n])n++;return n;}
static void clear_panel(fm01_panel *p) {p->total=0;p->file_count=0;p->dir_count=0;p->loaded=0;}

int fm01_refresh(fm01_model *m,unsigned which) {
    fm01_panel *p;
    uint32_t n=0,total=0,files=0,dirs=0;
    int rc;
    if (!m || !m->list || which>1u) return -1;
    p=&m->panels[which];
    /* First query refreshes counts and clamps a cursor after removals. */
    rc=m->list(m->fs_context,p->path,0,0,p->entries,&n,&total,&files,&dirs);
    if (rc) {clear_panel(p);p->last_error=rc;return rc;}
    p->total=total;p->file_count=files;p->dir_count=dirs;
    if (p->total==0) {p->cursor=0;p->first=0;p->loaded=0;p->last_error=0;return 0;}
    if (p->cursor>=total) p->cursor=total-1;
    p->first=(p->cursor/FM01_PAGE_ROWS)*FM01_PAGE_ROWS;
    rc=m->list(m->fs_context,p->path,p->first,FM01_PAGE_ROWS,p->entries,&n,&total,&files,&dirs);
    if (rc || n>FM01_PAGE_ROWS || n==0 || total!=p->total ||
        files!=p->file_count || dirs!=p->dir_count) {
        clear_panel(p);p->last_error=rc?rc:-2;return p->last_error;
    }
    p->loaded=n;p->last_error=0;return 0;
}
void fm01_init(fm01_model *m,fm01_list_page list,void *ctx) {
    if(!m)return;
    /* No heap/standard C runtime required by the model. */
    unsigned char *b=(unsigned char*)m;size_t i;
    for(i=0;i<sizeof(*m);i++)b[i]=0;
    m->list=list;m->fs_context=ctx;m->sd_present=1;
    if(list) {fm01_refresh(m,0);fm01_refresh(m,1);}
}
void fm01_select_panel(fm01_model *m,unsigned which) {
    if(m && which<2)m->active=(uint8_t)which;
}
const fm01_entry *fm01_selected(const fm01_model *m,unsigned which) {
    const fm01_panel *p;
    if(!m || which>1)return 0;
    p=&m->panels[which];
    if(!p->loaded || p->cursor<p->first || p->cursor-p->first>=p->loaded)return 0;
    return &p->entries[p->cursor-p->first];
}
static void move_cursor(fm01_model *m,int delta) {
    fm01_panel *p=&m->panels[m->active];
    uint32_t old=p->cursor;
    if(!p->total)return;
    if(delta<0 && p->cursor>0)p->cursor--;
    if(delta>0 && p->cursor+1<p->total)p->cursor++;
    if(old!=p->cursor && (p->cursor<p->first || p->cursor-p->first>=p->loaded)) {
        if (fm01_refresh(m,m->active))p->cursor=old;
    }
}
static int valid_segment(const char *s) {
    size_t i=0;
    if(!s || !s[0])return 0;
    if(s[0]=='.' && (s[1]==0 || (s[1]=='.' && s[2]==0)))return 0;
    for(;s[i];i++)if(s[i]=='/' || s[i]=='\\' || (unsigned char)s[i]<32)return 0;
    return 1;
}
int fm01_enter(fm01_model *m) {
    fm01_panel *p;const fm01_entry *e;size_t oldlen,len;
    if(!m)return -1;
    p=&m->panels[m->active];e=fm01_selected(m,m->active);
    if(!e || e->type!=FM01_ENTRY_DIR)return 1; /* leaf; opening files is FM-02+ */
    if(p->depth==FM01_MAX_DEPTH || !valid_segment(e->name))return -2;
    oldlen=bytes_len(p->path);len=bytes_len(e->name);
    if(oldlen+len+(oldlen?1u:0u)>=FM01_PATH_MAX)return -3;
    p->saved_cursor[p->depth++]=p->cursor;
    if(oldlen)p->path[oldlen++]='/';
    bytes_copy(&p->path[oldlen],e->name,FM01_PATH_MAX-oldlen);
    p->cursor=0;
    if (fm01_refresh(m,m->active)) {
        p->path[oldlen-(oldlen!=0 && p->path[oldlen-1]=='/')]=0;
        p->depth--;
        p->cursor=p->saved_cursor[p->depth];
        fm01_refresh(m,m->active);
        return -4;
    }
    return 0;
}
int fm01_up(fm01_model *m) {
    fm01_panel *p;size_t n;
    if(!m)return -1;
    p=&m->panels[m->active];
    if(!p->depth)return 1;
    n=bytes_len(p->path);
    while(n && p->path[n-1]!='/')n--;
    if(n)p->path[n-1]=0;else p->path[0]=0;
    p->depth--;
    p->cursor=p->saved_cursor[p->depth];
    return fm01_refresh(m,m->active);
}
void fm01_buttons(fm01_model *m,uint32_t pressed) {
    if(!m)return;
    if(pressed & FM01_KEY_LEFT)fm01_select_panel(m,0);
    else if(pressed & FM01_KEY_RIGHT)fm01_select_panel(m,1);
    else if(pressed & FM01_KEY_SELECT)fm01_select_panel(m,!m->active);
    else if(pressed & FM01_KEY_UP)move_cursor(m,-1);
    else if(pressed & FM01_KEY_DOWN)move_cursor(m,1);
    else if(pressed & FM01_KEY_A)(void)fm01_enter(m);
    else if(pressed & FM01_KEY_B)(void)fm01_up(m);
    /* START is reserved for the actions menu (FM-02); no write operations in FM-01. */
}
