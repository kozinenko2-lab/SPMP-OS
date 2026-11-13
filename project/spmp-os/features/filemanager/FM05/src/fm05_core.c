/* SPDX-License-Identifier: MIT */
#include "spmp_fm05.h"
#include <stddef.h>
#include <stdint.h>

static size_t size_limited(const char *s,size_t max){size_t n=0;if(!s)return max;while(n<max&&s[n])n++;return n;}
static void bytes_copy(void *dest,const void *src,unsigned n){unsigned i;char*d=(char*)dest;const char*s=(const char*)src;for(i=0;i<n;i++)d[i]=s[i];}
static int utf8_char(const char *p,unsigned n,uint32_t *cp){
    unsigned v,k,j,min;
    if(!n)return 0;
    v=(uint8_t)p[0];
    if(v<0x80u){*cp=v;return 1;}
    if(v>=0xc2u&&v<=0xdfu){k=2u;min=0x80u;v&=31u;}
    else if(v>=0xe0u&&v<=0xefu){k=3u;min=0x800u;v&=15u;}
    else if(v>=0xf0u&&v<=0xf4u){k=4u;min=0x10000u;v&=7u;}
    else return -1;
    if(k>n)return -1;
    for(j=1u;j<k;j++){
        uint32_t c=(uint8_t)p[j];if((c&0xc0u)!=0x80u)return -1;
        v=(v<<6u)|(c&63u);
    }
    if(v<min||v>0x10ffffu||(v>=0xd800u&&v<=0xdfffu))return -1;
    *cp=v;return (int)k;
}
int fm05_utf8_valid(const char *text,unsigned length){
    unsigned i=0;uint32_t cp;
    if(!text)return 0;
    while(i<length){int k=utf8_char(text+i,length-i,&cp);
        if(k<1||cp==0u||cp==0xfffeu||cp==0xffffu)return 0;
        if(cp<32u&&cp!=9u&&cp!=10u&&cp!=13u)return 0;
        i+=(unsigned)k;
    }
    return 1;
}
static int eq_ci(const char *s,const char *name){
    unsigned i=0;for(;;i++){unsigned a=(uint8_t)s[i],b=(uint8_t)name[i];
        if(a>='A'&&a<='Z')a+=32;if(b>='A'&&b<='Z')b+=32;
        if(a!=b)return 0;if(!a)return 1;
    }
}
int fm05_extension_supported(const char *path){
    const char *p,*dot=0;
    if(!path)return 0;
    for(p=path;*p;p++){if(*p=='/'||*p=='\\')dot=0;else if(*p=='.')dot=p;}
    if(!dot)return 0;
    return eq_ci(dot,".txt")||eq_ci(dot,".log")||eq_ci(dot,".ini")||eq_ci(dot,".cfg")||eq_ci(dot,".xml")||eq_ci(dot,".md")||eq_ci(dot,".csv")||eq_ci(dot,".json")||eq_ci(dot,".sh");
}
static unsigned next(const fm05_doc *m,unsigned pos){
    uint32_t cp;int n;
    if(pos>=m->length)return m->length;
    if(m->text[pos]=='\r'&&pos+1u<m->length&&m->text[pos+1u]=='\n')return pos+2u;
    n=utf8_char(m->text+pos,m->length-pos,&cp);
    return n>0?pos+(unsigned)n:pos+1u;
}
static unsigned prev(const fm05_doc *m,unsigned pos){
    if(pos==0)return 0;
    if(pos>=2u&&m->text[pos-2u]=='\r'&&m->text[pos-1u]=='\n')return pos-2u;
    pos--;
    while(pos>0u&&((uint8_t)m->text[pos]&0xc0u)==0x80u)pos--;
    return pos;
}
static unsigned line_start(const fm05_doc *m,unsigned pos){
    while(pos>0u&&m->text[pos-1u]!='\n')pos--;
    return pos;
}
static unsigned line_end(const fm05_doc *m,unsigned pos){
    while(pos<m->length&&m->text[pos]!='\n')pos++;
    if(pos>0u&&m->text[pos-1u]=='\r')pos--;
    return pos;
}
unsigned fm05_line_start_offset(const fm05_doc *m,unsigned target){
    unsigned l=0,p=0;
    if(!m||m->state==FM05_CLOSED)return 0;
    while(l<target&&p<m->length){while(p<m->length&&m->text[p]!='\n')p++;if(p<m->length)p++;l++;}
    return p;
}
unsigned fm05_caret_line(const fm05_doc *m){unsigned p,n=0;if(!m)return 0;for(p=0;p<m->caret&&p<m->length;p++)n+=(m->text[p]=='\n');return n;}
unsigned fm05_caret_column(const fm05_doc *m){unsigned p,c=0;if(!m)return 0;p=line_start(m,m->caret);while(p<m->caret&&m->text[p]!='\n'&&m->text[p]!='\r'){p=next(m,p);c++;}return c;}
static void update_lines(fm05_doc *m){unsigned i,n=1,crlf=0;for(i=0;i<m->length;i++){if(m->text[i]=='\n'){n++;if(i&&m->text[i-1]=='\r')crlf++;}}m->lines=n;m->crlf=(uint8_t)(crlf>0u);}
static void ensure_visible(fm05_doc *m){
    unsigned line=fm05_caret_line(m),col=fm05_caret_column(m);
    if(line<m->top_line)m->top_line=line;
    else if(line>=m->top_line+FM05_VIEW_ROWS)m->top_line=line-FM05_VIEW_ROWS+1u;
    if(col<m->first_col)m->first_col=col;
    else if(col>=m->first_col+FM05_VIEW_COLS)m->first_col=col-FM05_VIEW_COLS+1u;
}
void fm05_init(fm05_doc *m,fm05_fs fs){unsigned i;if(!m)return;for(i=0;i<sizeof(*m);i++)((uint8_t*)m)[i]=0;m->fs=fs;}
void fm05_set_translation(fm05_doc *m,fm05_translate translate,void *context){
    if(!m)return;m->translate=translate;m->translation_context=context;
}
const char *fm05_localize(const fm05_doc *m,unsigned token){
    static const char *const ru[]={"РЕДАКТОР","Стр","Поз","байт","Отменить изменения?",
        "A: Не сохранять  B: Назад","A: Клавиши SEL: Поиск START+SEL: Сохр."};
    static const char *const en[]={"EDITOR","Ln","Col","bytes","Discard unsaved changes?",
        "A: Discard   B: Keep","A: Keyboard SEL: Search START+SEL: Save"};
    const char *v;
    if(token>FM05_TX_CONTROLS)return "";
    v=m&&m->translate?m->translate(m->translation_context,token):0;
    return v?v:((m&&m->locale)?en:ru)[token];
}
static int safe_path(const char *p,unsigned n){
    unsigned i,component=0;
    if(!n||p[0]=='/'||p[n-1]=='/')return 0;
    for(i=0;i<=n;i++){
        if(p[i]=='/'||p[i]=='\0'){
            if(i==component||(i-component==1u&&p[component]=='.')||
               (i-component==2u&&p[component]=='.'&&p[component+1u]=='.'))return 0;
            component=i+1u;
        }else if((uint8_t)p[i]<32u||p[i]=='\\'||p[i]==':')return 0;
    }
    return 1;
}
int fm05_open(fm05_doc *m,const char *path,unsigned locale){
    unsigned n=0,i,len;uint64_t rev=0;int rc;
    if(!m||!m->fs.read||!path)return -1;
    len=(unsigned)size_limited(path,FM01_PATH_MAX);
    if(!len||len>=FM01_PATH_MAX||!safe_path(path,len)||!fm05_extension_supported(path)){m->last_error=FM05_ERR_PATH;return FM05_ERR_PATH;}
    /* Even if new loading fails, the prior dirty buffer must be preserved. */
    if(m->state!=FM05_CLOSED&&m->dirty){m->last_error=FM05_ERR_CONFLICT;return FM05_ERR_CONFLICT;}
    rc=m->fs.read(m->fs.context,path,m->text,FM05_TEXT_CAP,&n,&rev);
    if(rc){m->last_error=rc;m->state=FM05_CLOSED;return rc;}
    if(n>FM05_TEXT_CAP){m->last_error=FM05_ERR_TOO_LARGE;m->state=FM05_CLOSED;return FM05_ERR_TOO_LARGE;}
    if(!fm05_utf8_valid(m->text,n)){m->last_error=FM05_ERR_ENCODING;m->state=FM05_CLOSED;return FM05_ERR_ENCODING;}
    m->text[n]=0;m->length=n;m->caret=0;m->top_line=0;m->first_col=0;m->goal_col=0;
    m->revision=rev;m->locale=(uint8_t)(locale!=0u);m->dirty=0;m->state=FM05_OPEN;m->undo_valid=0;m->read_only=(uint8_t)(!m->fs.write);
    m->search[0]=0;m->search_wrapped=0;m->last_error=0;
    for(i=0;i<len;i++)m->path[i]=path[i];m->path[len]=0;
    update_lines(m);return 0;
}
int fm05_open_selected(fm05_doc *m,const fm01_model *browser){
    const fm01_entry *e;const fm01_panel *pane;size_t a,b;
    char path[FM01_PATH_MAX];
    if(!m||!browser)return FM05_ERR_PATH;
    pane=&browser->panels[browser->active];e=fm01_selected(browser,browser->active);
    if(!e||e->type!=FM01_ENTRY_FILE)return FM05_ERR_NO_FILE;
    a=size_limited(pane->path,FM01_PATH_MAX);b=size_limited(e->name,FM01_NAME_MAX);
    if(a==FM01_PATH_MAX||b==FM01_NAME_MAX||a+b+2u>FM01_PATH_MAX)return FM05_ERR_PATH;
    for(size_t i=0;i<a;i++)path[i]=pane->path[i];
    if(a)path[a++]='/';for(size_t i=0;i<b;i++)path[a+i]=e->name[i];path[a+b]=0;
    return fm05_open(m,path,browser->locale);
}
/* Replace the span [pos, pos+remove) with n bytes. Undo stores ONLY the last action. */
static int edit_span(fm05_doc *m,unsigned pos,unsigned remove,const char *insert,unsigned n,int record){
    unsigned i,j;
    if(!m||m->state!=FM05_OPEN||m->read_only)return FM05_ERR_READ_ONLY;
    if(pos>m->length||remove>m->length-pos||n>FM05_TEXT_CAP-(m->length-remove))return FM05_ERR_TOO_LARGE;
    if(record){m->undo_valid=0;if(remove<=sizeof(m->undo_bytes)&&n<=255u){
        for(i=0;i<remove;i++)m->undo_bytes[i]=m->text[pos+i];
        m->undo_old_size=(uint16_t)remove;m->undo_new_size=(uint16_t)n;m->undo_pos=pos;m->undo_valid=1;
    }}
    if(n>remove){for(i=m->length+1u;i>pos+remove;i--)m->text[i+n-remove-1u]=m->text[i-1u];}
    else if(remove>n){for(i=pos+remove;i<=m->length;i++)m->text[i-(remove-n)]=m->text[i];}
    for(j=0;j<n;j++)m->text[pos+j]=insert[j];
    m->length=m->length-remove+n;m->text[m->length]=0;m->caret=pos+n;
    m->dirty=1;update_lines(m);ensure_visible(m);return 0;
}
int fm05_insert(fm05_doc *m,const char *utf8,unsigned n){
    unsigned i;
    if(!m||!utf8||!n||m->state!=FM05_OPEN)return -1;
    if(!fm05_utf8_valid(utf8,n))return FM05_ERR_ENCODING;
    for(i=0;i<n;i++)if(utf8[i]=='\r'||utf8[i]=='\n')return FM05_ERR_ENCODING; /* use newline() */
    return edit_span(m,m->caret,0,utf8,n,1);
}
int fm05_newline(fm05_doc *m){const char *s=(m&&m->crlf)?"\r\n":"\n";return m?edit_span(m,m->caret,0,s,m->crlf?2u:1u,1):-1;}
int fm05_backspace(fm05_doc *m){unsigned p;if(!m||!m->caret)return 0;p=prev(m,m->caret);return edit_span(m,p,m->caret-p,"",0,1);}
int fm05_delete(fm05_doc *m){unsigned p;if(!m||m->caret>=m->length)return 0;p=next(m,m->caret);return edit_span(m,m->caret,p-m->caret,"",0,1);}
int fm05_undo(fm05_doc *m){int rc;if(!m||!m->undo_valid)return -1;
    rc=edit_span(m,m->undo_pos,m->undo_new_size,m->undo_bytes,m->undo_old_size,0);
    if(!rc)m->undo_valid=0;return rc;
}
void fm05_left(fm05_doc *m){if(m&&m->state==FM05_OPEN){m->caret=prev(m,m->caret);m->goal_col=fm05_caret_column(m);ensure_visible(m);}}
void fm05_right(fm05_doc *m){if(m&&m->state==FM05_OPEN){m->caret=next(m,m->caret);m->goal_col=fm05_caret_column(m);ensure_visible(m);}}
void fm05_line_start(fm05_doc *m){if(m&&m->state==FM05_OPEN){m->caret=line_start(m,m->caret);m->goal_col=0;ensure_visible(m);}}
void fm05_line_end(fm05_doc *m){if(m&&m->state==FM05_OPEN){m->caret=line_end(m,m->caret);m->goal_col=fm05_caret_column(m);ensure_visible(m);}}
static void vertical(fm05_doc *m,int dir){
    unsigned current,begin,end,p,col=0,goal;
    if(!m||m->state!=FM05_OPEN)return;
    current=fm05_caret_line(m);
    if(dir<0&&current==0)return;
    if(dir>0&&current+1u>=m->lines)return;
    goal=m->goal_col;
    begin=fm05_line_start_offset(m,dir<0?current-1u:current+1u);
    end=line_end(m,begin);p=begin;
    while(p<end&&col<goal){p=next(m,p);col++;}
    m->caret=p;ensure_visible(m);
}
void fm05_up(fm05_doc *m){vertical(m,-1);}
void fm05_down(fm05_doc *m){vertical(m,1);}
void fm05_page(fm05_doc *m,int dir){unsigned i;for(i=0;m&&i<FM05_VIEW_ROWS;i++)vertical(m,dir<0?-1:1);}
static unsigned substr(const char *t,unsigned len,const char *q,unsigned qlen,unsigned start){
    unsigned i,j;if(!qlen||qlen>len)return len;
    for(i=start;i+qlen<=len;i++){
        if(((uint8_t)t[i]&0xc0u)==0x80u)continue;
        for(j=0;j<qlen&&t[i+j]==q[j];j++){}
        if(j==qlen)return i;
    }
    return len;
}
int fm05_find(fm05_doc *m,const char *q,int wrap){
    unsigned n,pos;
    if(!m||m->state!=FM05_OPEN||!q)return -1;
    n=(unsigned)size_limited(q,FM05_SEARCH_CAP);
    if(!n||n>=FM05_SEARCH_CAP||!fm05_utf8_valid(q,n))return FM05_ERR_ENCODING;
    bytes_copy(m->search,q,n);m->search[n]=0;
    pos=substr(m->text,m->length,q,n,m->caret<m->length?next(m,m->caret):m->length);
    m->search_wrapped=0;
    if(pos==m->length&&wrap){pos=substr(m->text,m->length,q,n,0);if(pos!=m->length)m->search_wrapped=1;}
    if(pos==m->length)return 0;
    m->caret=pos;m->goal_col=fm05_caret_column(m);ensure_visible(m);return 1;
}
int fm05_save(fm05_doc *m){uint64_t rev;int rc;
    if(!m||m->state!=FM05_OPEN||!m->fs.write)return FM05_ERR_READ_ONLY;
    if(!m->dirty)return 0;
    rc=m->fs.write(m->fs.context,m->path,m->text,m->length,m->revision,&rev);
    if(rc){m->last_error=rc;return rc;}
    m->revision=rev;m->dirty=0;m->undo_valid=0;m->last_error=0;return 0;
}
int fm05_request_close(fm05_doc *m){if(!m)return -1;if(m->dirty){m->state=FM05_DISCARD_CONFIRM;return 0;}m->state=FM05_CLOSED;return 1;}
void fm05_confirm_close(fm05_doc *m,int discard){if(!m||m->state!=FM05_DISCARD_CONFIRM)return;m->state=discard?FM05_CLOSED:FM05_OPEN;if(discard)m->dirty=0;}
int fm05_open_keyboard(fm05_doc *m,fm04_model *k,fm05_input_mode mode){
    if(!m||!k||m->state!=FM05_OPEN)return -1;
    if(mode==FM05_INPUT_TEXT)return fm04_open(k,"",FM04_LINE,m->locale);
    if(mode==FM05_INPUT_SEARCH)return fm04_open(k,m->search,FM04_LINE,m->locale);
    return -1;
}
int fm05_finish_keyboard(fm05_doc *m,const fm04_model *k,fm05_input_mode mode){
    if(!m||!k||k->state!=FM04_ACCEPTED)return -1;
    if(mode==FM05_INPUT_TEXT)return k->bytes?fm05_insert(m,k->value,k->bytes):0;
    if(mode==FM05_INPUT_SEARCH)return fm05_find(m,k->value,1);
    return -1;
}
void fm05_buttons(fm05_doc *m,uint32_t keys){
    if(!m||m->state!=FM05_OPEN)return;
    if((keys&(FM01_KEY_START|FM01_KEY_SELECT))==(FM01_KEY_START|FM01_KEY_SELECT)){(void)fm05_save(m);return;}
    if(keys&FM01_KEY_B){(void)fm05_request_close(m);return;}
    if(keys&FM01_KEY_UP)fm05_up(m);
    else if(keys&FM01_KEY_DOWN)fm05_down(m);
    else if(keys&FM01_KEY_LEFT)fm05_left(m);
    else if(keys&FM01_KEY_RIGHT)fm05_right(m);
}
