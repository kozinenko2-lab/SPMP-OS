/* SPDX-License-Identifier: MIT */
#include "spmp_fm04.h"
#include <stddef.h>
#include <stdint.h>

static const char *const latin[33] = {
    "q","w","e","r","t","y","u","i","o","p","1",
    "a","s","d","f","g","h","j","k","l","2","3",
    "z","x","c","v","b","n","m",".",",","-","_"
};
static const char *const russian[33] = {
    "й","ц","у","к","е","н","г","ш","щ","з","х",
    "ф","ы","в","а","п","р","о","л","д","ж","э",
    "я","ч","с","м","и","т","ь","б","ю","ё","ъ"
};
static const char *const russian_upper[33] = {
    "Й","Ц","У","К","Е","Н","Г","Ш","Щ","З","Х",
    "Ф","Ы","В","А","П","Р","О","Л","Д","Ж","Э",
    "Я","Ч","С","М","И","Т","Ь","Б","Ю","Ё","Ъ"
};
static const char *const symbols[33] = {
    "0","1","2","3","4","5","6","7","8","9",".",
    "!","@","#","$","%","^","&","*","(",")","-",
    "_","+","=",":",";","?","'","\"","[","]","~"
};

void fm04_set_translation(fm04_model *k,fm04_translate callback,void *context) {
    if(!k)return;
    k->translate=callback;k->translation_context=context;
}
const char *fm04_localize(const fm04_model *k,unsigned token) {
    static const char *const ru[]={"Переименовать","Ввод текста","Ошибка имени / ввода",
        "A ввод B стереть SEL символы START OK","Рг","ЯЗ","#+","ПР","<",">","BS","DEL","CLR","ОТМ","OK"};
    static const char *const en[]={"Rename","Text input","Invalid name / input",
        "A key B erase SEL symbols START OK","SH","LANG","#+","SP","<",">","BS","DEL","CLR","ESC","OK"};
    const char *v;
    if(!k||token>FM04_TEXT_OK)return "";
    v=k->translate?k->translate(k->translation_context,token):0;
    return v?v:(k->locale?en:ru)[token];
}
static size_t nbytes(const char *s, size_t cap) {
    size_t i=0;
    if(!s)return cap;
    while(i<cap && s[i])i++;
    return i;
}
static int char_decode(const char *s, size_t remain, uint32_t *cp) {
    uint32_t c, val, min;
    unsigned count, i;
    if(!remain)return 0;
    c=(uint8_t)s[0];
    if(c<0x80){*cp=c;return 1;}
    if(c>=0xc2 && c<=0xdf){count=2;val=c&31u;min=0x80u;}
    else if(c>=0xe0 && c<=0xef){count=3;val=c&15u;min=0x800u;}
    else if(c>=0xf0 && c<=0xf4){count=4;val=c&7u;min=0x10000u;}
    else return -1;
    if(count>remain)return -1;
    for(i=1;i<count;i++){
        c=(uint8_t)s[i];
        if((c&0xc0u)!=0x80u)return -1;
        val=(val<<6)|(c&63u);
    }
    if(val<min||val>0x10ffffu||(val>=0xd800u&&val<=0xdfffu))return -1;
    *cp=val;
    return (int)count;
}
int fm04_utf8_valid(const char *s, unsigned max_bytes) {
    size_t n=nbytes(s,max_bytes), i=0;
    uint32_t cp;
    if(n==max_bytes)return 0;
    while(i<n){int k=char_decode(s+i,n-i,&cp);if(k<=0||cp==0)return 0;i+=(unsigned)k;}
    return 1;
}
static int ascii_up(unsigned c) {return c>='a'&&c<='z'? (int)c-32:(int)c;}
/* Portable basenames for FAT/vFAT, not just POSIX. */
int fm04_filename_valid(const char *s) {
    size_t n=nbytes(s,FM01_NAME_MAX),i=0,stem=0,j;
    const char *reserved[] = {"CON","PRN","AUX","NUL","COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","LPT1","LPT2","LPT3","LPT4","LPT5","LPT6","LPT7","LPT8","LPT9"};
    uint32_t cp;
    if(!n||n==FM01_NAME_MAX||!fm04_utf8_valid(s,FM01_NAME_MAX))return 0;
    if(s[n-1]==' '||s[n-1]=='.')return 0;
    if(n==1 && s[0]=='.')return 0;
    if(n==2 && s[0]=='.'&&s[1]=='.')return 0;
    while(i<n){int k=char_decode(s+i,n-i,&cp);if(k<=0)return 0;
        if(cp<32||cp==127||cp=='/'||cp=='\\'||cp==':'||cp=='*'||cp=='?'||cp=='"'||cp=='<'||cp=='>'||cp=='|')return 0;
        i+=(unsigned)k;
    }
    while(stem<n&&s[stem]!='.')stem++;
    for(j=0;j<sizeof(reserved)/sizeof(reserved[0]);j++){
        const char *r=reserved[j];size_t z=0;
        while(z<stem&&r[z]&&ascii_up((unsigned char)s[z])==(unsigned char)r[z])z++;
        if(z==stem&&r[z]==0)return 0;
    }
    return 1;
}
static int cursor_boundary(const fm04_model *m) {
    if(m->cursor>m->bytes||m->bytes>=FM01_NAME_MAX||m->value[m->bytes]!=0)return 0;
    if(m->cursor<m->bytes&&((uint8_t)m->value[m->cursor]&0xc0u)==0x80u)return 0;
    return fm04_utf8_valid(m->value,FM01_NAME_MAX);
}
int fm04_open(fm04_model *k, const char *initial, fm04_mode mode, unsigned locale) {
    size_t i,n;
    if(!k||!initial||(mode!=FM04_FILENAME&&mode!=FM04_LINE))return -1;
    n=nbytes(initial,FM01_NAME_MAX);
    if(n==FM01_NAME_MAX||!fm04_utf8_valid(initial,FM01_NAME_MAX))return -2;
    for(i=0;i<n;i++)if((uint8_t)initial[i]<32u)return -3;
    for(i=0;i<sizeof(*k);i++)((uint8_t*)k)[i]=0;
    for(i=0;i<=n;i++)k->value[i]=initial[i];
    k->bytes=(uint16_t)n;k->cursor=(uint16_t)n;
    k->locale=locale?1u:0u;k->layout=locale?FM04_EN:FM04_RU;
    k->mode=(uint8_t)mode;k->state=FM04_EDITING;k->selection=0;
    return 0;
}
int fm04_open_rename(fm04_model *k,fm02_model *actions) {
    const fm01_entry *item;int rc;
    if(!actions||actions->state!=FM02_RENAME_INPUT_REQUIRED||!actions->browser)return -1;
    item=fm01_selected(actions->browser,actions->browser->active);
    if(!item)return -2;
    rc=fm04_open(k,item->name,FM04_FILENAME,actions->browser->locale);
    if(rc)return rc;
    k->rename_target=actions;
    return 0;
}
int fm04_insert(fm04_model *k,const char *s) {
    size_t len,i;uint32_t cp;int length;
    if(!k||k->state!=FM04_EDITING||!s)return -1;
    len=nbytes(s,5);if(len==0||len>=5)return -2;
    length=char_decode(s,len,&cp);if(length<=0||(size_t)length!=len)return -2;
    if(cp<32u||cp==127u)return -3;
    if(k->mode==FM04_FILENAME && (cp=='/'||cp=='\\'||cp==':'||cp=='*'||cp=='?'||cp=='"'||cp=='<'||cp=='>'||cp=='|'))return -3;
    if(k->bytes+len>=FM01_NAME_MAX)return -4;
    if(!cursor_boundary(k))return -5;
    for(i=k->bytes+1u;i>k->cursor;i--)k->value[i+len-1u]=k->value[i-1u];
    for(i=0;i<len;i++)k->value[k->cursor+i]=s[i];
    k->cursor=(uint16_t)(k->cursor+len);k->bytes=(uint16_t)(k->bytes+len);
    k->error=0;
    return 0;
}
int fm04_backspace(fm04_model *k) {
    size_t a,i;
    if(!k||k->state!=FM04_EDITING||!cursor_boundary(k))return -1;
    if(!k->cursor)return 0;
    a=(size_t)k->cursor-1u;
    while(a>0u&&((uint8_t)k->value[a]&0xc0u)==0x80u)a--;
    for(i=a;i<=k->bytes-(size_t)k->cursor+a;i++)k->value[i]=k->value[i+k->cursor-a];
    k->bytes=(uint16_t)(k->bytes-(k->cursor-a));k->cursor=(uint16_t)a;k->error=0;
    return 1;
}
int fm04_delete(fm04_model *k) {
    uint32_t cp;int length;size_t i;
    if(!k||k->state!=FM04_EDITING||!cursor_boundary(k))return -1;
    if(k->cursor>=k->bytes)return 0;
    length=char_decode(k->value+k->cursor,k->bytes-k->cursor,&cp);
    if(length<=0)return -1;
    for(i=k->cursor;i+((unsigned)length)<=k->bytes;i++)k->value[i]=k->value[i+(unsigned)length];
    k->bytes=(uint16_t)(k->bytes-(unsigned)length);k->error=0;
    return 1;
}
void fm04_move_cursor(fm04_model *k,int direction) {
    unsigned pos;
    if(!k||k->state!=FM04_EDITING||!cursor_boundary(k))return;
    if(direction<0){if(!k->cursor)return;pos=(unsigned)k->cursor-1u;
        while(pos>0&&((uint8_t)k->value[pos]&0xc0u)==0x80u)pos--;
        k->cursor=(uint16_t)pos;
    }else if(direction>0){uint32_t cp;int length;
        if(k->cursor>=k->bytes)return;
        length=char_decode(k->value+k->cursor,k->bytes-k->cursor,&cp);
        if(length>0)k->cursor=(uint16_t)(k->cursor+(unsigned)length);
    }
}
const char *fm04_key_label(const fm04_model *k,unsigned index) {
    const char *s;
    if(!k||index>=FM04_KEYS)return "";
    if(index>=33u)return fm04_localize(k,FM04_TEXT_SHIFT+(index-33u));
    if(k->layout==FM04_RU)return (k->shift?russian_upper:russian)[index];
    if(k->layout==FM04_SYMBOLS)return symbols[index];
    s=latin[index];
    if(k->shift && s[0]>='a'&&s[0]<='z'){
        static const char *const upper[26]={"Q","W","E","R","T","Y","U","I","O","P","A","S","D","F","G","H","J","K","L","Z","X","C","V","B","N","M"};
        unsigned j;
        if(index<10u)return upper[index];
        if(index>=11u&&index<20u)return upper[index-1u];
        for(j=0;j<7u;j++)if(index==22u+j)return upper[19u+j];
    }
    return s;
}
fm04_event fm04_finish(fm04_model *k) {
    if(!k||k->state!=FM04_EDITING)return FM04_ERROR;
    if(k->mode==FM04_FILENAME && !fm04_filename_valid(k->value)){k->error=1;return FM04_ERROR;}
    if(!fm04_utf8_valid(k->value,FM01_NAME_MAX)){k->error=2;return FM04_ERROR;}
    if(k->rename_target){
        int rc=fm02_supply_rename(k->rename_target,k->value);
        if(rc){k->error=3;return FM04_ERROR;}
    }
    k->state=FM04_ACCEPTED;k->error=0;
    return FM04_ACCEPT;
}
void fm04_cancel(fm04_model *k) {
    if(!k||k->state!=FM04_EDITING)return;
    if(k->rename_target && k->rename_target->state==FM02_RENAME_INPUT_REQUIRED)
        k->rename_target->state=FM02_IDLE;
    k->state=FM04_CANCELLED;
}
fm04_event fm04_activate(fm04_model *k,unsigned index) {
    int rc;
    const char *label;
    if(!k||k->state!=FM04_EDITING||index>=FM04_KEYS)return FM04_ERROR;
    k->selection=(uint8_t)index;
    if(index<33u){
        label=fm04_key_label(k,index);
        rc=fm04_insert(k,label);
        if(rc){k->error=1;return FM04_ERROR;}
        return FM04_CHANGED;
    }
    switch(index-33u){
        case 0:k->shift^=1u;return FM04_CHANGED;
        case 1:k->layout=k->layout==FM04_RU?FM04_EN:FM04_RU;return FM04_CHANGED;
        case 2:k->layout=k->layout==FM04_SYMBOLS?(k->locale?FM04_EN:FM04_RU):FM04_SYMBOLS;return FM04_CHANGED;
        case 3:rc=fm04_insert(k," ");return rc?FM04_ERROR:FM04_CHANGED;
        case 4:fm04_move_cursor(k,-1);return FM04_CHANGED;
        case 5:fm04_move_cursor(k,1);return FM04_CHANGED;
        case 6:rc=fm04_backspace(k);return rc<0?FM04_ERROR:FM04_CHANGED;
        case 7:rc=fm04_delete(k);return rc<0?FM04_ERROR:FM04_CHANGED;
        case 8:k->bytes=0;k->cursor=0;k->value[0]=0;return FM04_CHANGED;
        case 9:fm04_cancel(k);return FM04_CANCEL;
        case 10:return fm04_finish(k);
        default:return FM04_ERROR;
    }
}
fm04_event fm04_buttons(fm04_model *k,uint32_t pressed) {
    unsigned row,col;
    if(!k||k->state!=FM04_EDITING)return FM04_NO_EVENT;
    if((pressed&(FM01_KEY_SELECT|FM01_KEY_B))==(FM01_KEY_SELECT|FM01_KEY_B)){
        fm04_cancel(k);return FM04_CANCEL;
    }
    if(pressed&FM01_KEY_START)return fm04_finish(k);
    if(pressed&FM01_KEY_SELECT){
        k->layout=k->layout==FM04_SYMBOLS?(k->locale?FM04_EN:FM04_RU):FM04_SYMBOLS;
        return FM04_CHANGED;
    }
    if(pressed&FM01_KEY_B)return fm04_activate(k,39u);
    if(pressed&FM01_KEY_A)return fm04_activate(k,k->selection);
    row=k->selection/FM04_COLS;col=k->selection%FM04_COLS;
    if(pressed&FM01_KEY_UP)row=(row+FM04_ROWS-1u)%FM04_ROWS;
    else if(pressed&FM01_KEY_DOWN)row=(row+1u)%FM04_ROWS;
    else if(pressed&FM01_KEY_LEFT)col=(col+FM04_COLS-1u)%FM04_COLS;
    else if(pressed&FM01_KEY_RIGHT)col=(col+1u)%FM04_COLS;
    else return FM04_NO_EVENT;
    k->selection=(uint8_t)(row*FM04_COLS+col);
    return FM04_CHANGED;
}
