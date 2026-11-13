#include "spmp_ro_fs.h"
#include <string.h>
int spmp_ro_path_valid(const char *path){
    if(!path || path[0]!='/' || !path[1])return path && path[0]=='/' && !path[1];
    size_t len=0,segment=1;
    while(path[len]){
        unsigned char c=(unsigned char)path[len];
        if(c<32 || c==127 || c=='\\' || c==':' || c=='*' || c=='?' ||
           c=='"' || c=='<' || c=='>' || c=='|')return 0;
        if(c=='/'){
            if(len && path[len-1]=='/')return 0;
            if(len>segment && len-segment==2 && path[segment]=='.' && path[segment+1]=='.')return 0;
            segment=len+1;
        }
        if(++len>=SPMP_FS_PATH_MAX)return 0;
    }
    if(len>1 && path[len-1]=='/')return 0;
    if(len>segment && len-segment==2 && path[segment]=='.' && path[segment+1]=='.')return 0;
    return 1;
}
static int safe_entry(const spmp_ro_entry *e){
    size_t i=0;
    if(!e->name[0] || e->name[0]=='/' || e->name[0]=='\\')return 0;
    for(;i<SPMP_FS_NAME_MAX;i++){
        unsigned char c=(unsigned char)e->name[i];
        if(!c)return !(i==1&&e->name[0]=='.') &&
                    !(i==2&&e->name[0]=='.'&&e->name[1]=='.') &&
                    (e->is_dir<=1);
        if(c<32 || c==127 || c=='/' || c=='\\')return 0;
    }
    return 0;
}
int spmp_ro_open_page(const spmp_ro_fs *fs,spmp_ro_panel *p,
    spmp_ro_panel *scratch,const char *path,size_t offset){
    if(!fs||!p||!scratch||p==scratch||!fs->confirm_readonly||
       !fs->read_directory||!spmp_ro_path_valid(path))return -1;
    if(fs->confirm_readonly(fs->context)!=1)return -1;
    /* Caller-provided scratch avoids a large temporary on the task stack. */
    memset(scratch,0,sizeof *scratch);
    size_t len=strlen(path);memcpy(scratch->path,path,len+1);
    size_t n=0,total=0,files=0;
    if(fs->read_directory(fs->context,path,offset,scratch->entries,
         SPMP_FS_MAX_ENTRIES,&n,&total,&files)!=0 ||
       n>SPMP_FS_MAX_ENTRIES || files>total || offset>total ||
       n>total-offset || (n==0 && offset<total))return -1;
    for(size_t i=0;i<n;i++)if(!safe_entry(&scratch->entries[i]))return -1;
    scratch->count=n;scratch->page_offset=offset;
    scratch->total_entries=total;scratch->total_files=files;
    *p=*scratch;return 0;
}
int spmp_ro_switch(spmp_ro_dual_panel *dual,unsigned new_active){
    if(!dual||new_active>=2)return -1;
    dual->active=new_active;return 0;
}
int spmp_ro_select(spmp_ro_dual_panel *dual,size_t index){
    if(!dual||dual->active>=2||index>=dual->panels[dual->active].count)return -1;
    dual->panels[dual->active].selected=index;return 0;
}
int spmp_ro_short_date(uint16_t fat_date,char out[9]){
    if(!out)return -1;
    unsigned day=fat_date&31u,month=(fat_date>>5)&15u,year=(fat_date>>9)&127u;
    if(!day||day>31||!month||month>12)return -1;
    unsigned yy=(80u+year)%100u;
    out[0]=(char)('0'+day/10);out[1]=(char)('0'+day%10);out[2]='.';
    out[3]=(char)('0'+month/10);out[4]=(char)('0'+month%10);out[5]='.';
    out[6]=(char)('0'+yy/10);out[7]=(char)('0'+yy%10);out[8]=0;
    return 0;
}
