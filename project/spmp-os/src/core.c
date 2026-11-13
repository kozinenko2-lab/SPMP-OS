#include "defos.h"
#include "spmp_cores.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int endswith(const char *name, const char *suffix) {
    size_t n=strlen(name), s=strlen(suffix);
    if (n<s) return 0;
    for(size_t i=0; i<s; ++i)
        if(tolower((unsigned char)name[n-s+i])!=tolower((unsigned char)suffix[i]))return 0;
    return 1;
}
defos_kind defos_classify(const char *name,int dir) {
    if(dir) return DEFOS_FILE_DIR;
    if(endswith(name,".nes"))return DEFOS_FILE_NES;
    if(endswith(name,".gb")||endswith(name,".gbc"))return DEFOS_FILE_GB;
    if(endswith(name,".gba"))return DEFOS_FILE_GBA;
    if(endswith(name,".sfc")||endswith(name,".smc"))return DEFOS_FILE_SNES;
    if(endswith(name,".md")||endswith(name,".gen"))return DEFOS_FILE_MD;
    if(endswith(name,".sms")||endswith(name,".gg")||endswith(name,".sg"))return DEFOS_FILE_SMS;
    if(endswith(name,".rom")||endswith(name,".cas"))return DEFOS_FILE_MSX;
    if(endswith(name,".z80")||endswith(name,".tzx"))return DEFOS_FILE_ZX;
    if(endswith(name,".d64")||endswith(name,".t64")||endswith(name,".prg"))return DEFOS_FILE_C64;
    if(endswith(name,".dsk"))return DEFOS_FILE_DISK;
    if(endswith(name,".tap"))return DEFOS_FILE_TAPE;
    if(endswith(name,".sna"))return DEFOS_FILE_SNAPSHOT;
    if(endswith(name,".cue")||endswith(name,".chd"))return DEFOS_FILE_PS1;
    if(endswith(name,".a26"))return DEFOS_FILE_ATARI;
    if(endswith(name,".wad")||endswith(name,".pk3")||endswith(name,".pak"))return DEFOS_FILE_WAD;
    if(endswith(name,".mp3")||endswith(name,".aac")||endswith(name,".flac")||endswith(name,".wma")||endswith(name,".ogg")||endswith(name,".wav")||endswith(name,".ape")||endswith(name,".m4a"))return DEFOS_FILE_AUDIO;
    if(endswith(name,".mp4")||endswith(name,".avi")||endswith(name,".mkv")||endswith(name,".mov")||endswith(name,".wmv")||endswith(name,".rmvb")||endswith(name,".3gp")||endswith(name,".mpg"))return DEFOS_FILE_VIDEO;
    if(endswith(name,".jpg")||endswith(name,".jpeg")||endswith(name,".png")||endswith(name,".gif")||endswith(name,".bmp")||endswith(name,".webp"))return DEFOS_FILE_IMAGE;
    if(endswith(name,".bin"))return DEFOS_FILE_BIN;
    if(endswith(name,".pce"))return DEFOS_FILE_PCE;
    if(endswith(name,".txt"))return DEFOS_FILE_TEXT;
    if(endswith(name,".elf")||endswith(name,".rap"))return DEFOS_FILE_PROGRAM;
    return DEFOS_FILE_UNKNOWN;
}
const char *defos_kind_name(defos_kind kind) {
    switch(kind) {
      case DEFOS_FILE_DIR:return "DIR";
      case DEFOS_FILE_NES:return "NES";
      case DEFOS_FILE_GB:return "GB/C";
      case DEFOS_FILE_GBA:return "GBA";
      case DEFOS_FILE_SNES:return "SNES";
      case DEFOS_FILE_MD:return "MD/BIN";
      case DEFOS_FILE_PCE:return "PCE";
      case DEFOS_FILE_TEXT:return "TEXT";
      case DEFOS_FILE_PROGRAM:return "ARM";
      case DEFOS_FILE_BIN:return "BIN";
      case DEFOS_FILE_AUDIO:return "AUDIO";
      case DEFOS_FILE_VIDEO:return "VIDEO";
      case DEFOS_FILE_IMAGE:return "IMAGE";
      case DEFOS_FILE_WAD:return "DATA";
      case DEFOS_FILE_MSX:return "MSX";
      case DEFOS_FILE_SMS:return "SMS/GG";
      case DEFOS_FILE_ATARI:return "2600";
      case DEFOS_FILE_ZX:return "ZX";
      case DEFOS_FILE_C64:return "C64";
      case DEFOS_FILE_DISK:return "DISK";
      case DEFOS_FILE_TAPE:return "TAPE";
      case DEFOS_FILE_SNAPSHOT:return "SNAP";
      case DEFOS_FILE_PS1:return "PS1";
      default:return "FILE";
    }
}
static int order(const defos_entry *a,const defos_entry *b) {
    if((a->kind==DEFOS_FILE_DIR)!=(b->kind==DEFOS_FILE_DIR))
        return a->kind==DEFOS_FILE_DIR ? -1:1;
    const unsigned char *x=(const unsigned char*)a->name, *y=(const unsigned char*)b->name;
    while(*x&&*y){int xx=tolower(*x++),yy=tolower(*y++);if(xx!=yy)return xx-yy;}
    return *x-*y;
}
int defos_refresh(defos_model *m) {
    if(!m || !m->list)return -1;
    int count=m->list(m->fs_context,m->relative,m->entries,DEFOS_MAX_ENTRIES);
    if(count<0){m->count=0;snprintf(m->message,sizeof m->message,"FAILED TO LIST DIRECTORY");return -1;}
    m->count=(size_t)count;
    for(size_t i=1;i<m->count;i++){
        defos_entry item=m->entries[i];size_t j=i;
        while(j>0 && order(&item,&m->entries[j-1])<0){m->entries[j]=m->entries[j-1];j--;}
        m->entries[j]=item;
    }
    if(m->count==0)m->selected=0;
    else if(m->selected>=m->count)m->selected=m->count-1;
    if(m->scroll>m->selected)m->scroll=m->selected;
    snprintf(m->message,sizeof m->message,"%u ITEMS",(unsigned)m->count);
    return 0;
}
void defos_init(defos_model *m,defos_list_fn list,void *ctx) {
    memset(m,0,sizeof(*m));m->list=list;m->fs_context=ctx;
    spmp_theme_default(&m->theme);
    strcpy(m->message,"SPMP-OS STAGE 2");
    defos_refresh(m);
}
void defos_move(defos_model *m,int change) {
    if(!m->count)return;
    long n=(long)m->selected+change;
    if(n<0)n=0;
    if(n>=(long)m->count)n=(long)m->count-1;
    m->selected=(size_t)n;
    if(m->selected<m->scroll)m->scroll=m->selected;
    if(m->selected>=m->scroll+12)m->scroll=m->selected-11;
}
int defos_enter(defos_model *m) {
    if(m->selected>=m->count)return -1;
    const defos_entry *e=&m->entries[m->selected];
    if(e->kind!=DEFOS_FILE_DIR){
        const spmp_core_desc *candidate[8];size_t total=0;
        spmp_match_result match=spmp_core_resolve(e->name,candidate,8,&total);
        if(match==SPMP_MATCH_AMBIGUOUS){
            snprintf(m->message,sizeof m->message,"SELECT CORE: %zu OPTIONS",total);
        } else if(match==SPMP_MATCH_UNIQUE){
            const spmp_core_desc *core=candidate[0];
            snprintf(m->message,sizeof m->message,"%s: %s",core->id,
                core->state==SPMP_CORE_HOST_TESTED?"HOST TEST ONLY":
                core->state==SPMP_CORE_SOURCE_AVAILABLE?"SOURCE ONLY":"RESEARCH ONLY");
        } else {
            snprintf(m->message,sizeof m->message,"%s: LAUNCH NOT IMPLEMENTED",defos_kind_name(e->kind));
        }
        return 1;
    }
    size_t a=strlen(m->relative),n=strlen(e->name);
    if(a+n+(a?1:0)>DEFOS_REL_MAX){strcpy(m->message,"PATH TOO LONG");return -1;}
    if(a)m->relative[a++]='/';
    memcpy(m->relative+a,e->name,n+1);
    m->selected=m->scroll=0;
    return defos_refresh(m);
}
void defos_back(defos_model *m) {
    char *slash=strrchr(m->relative,'/');
    if(slash)*slash=0;
    else m->relative[0]=0;
    m->selected=m->scroll=0;
    defos_refresh(m);
}
