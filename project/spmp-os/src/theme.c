#include "spmp_theme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int color(const char *s,uint16_t *out) {
    if(strlen(s)!=7 || s[0]!='#')return -1;
    unsigned x=0;
    for(int i=1;i<7;i++) {
        unsigned c=(unsigned char)s[i];
        if(!isxdigit(c))return -1;
        x=x*16+(c>='0'&&c<='9'?c-(unsigned)'0':(unsigned)tolower((int)c)-(unsigned)'a'+10u);
    }
    unsigned r=(x>>16)&255,g=(x>>8)&255,b=x&255;
    *out=(uint16_t)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
    return 0;
}
static char *trim(char *s) {
    while(isspace((unsigned char)*s))s++;
    size_t n=strlen(s);
    while(n && isspace((unsigned char)s[n-1]))s[--n]=0;
    return s;
}
void spmp_theme_default(spmp_theme *t) {
    *t=(spmp_theme){.background=0x1083,.panel=0x2108,.accent=0x05ba,
        .text=0xffff,.muted=0x9cf3,.selection=0x336e,.border=0x4208};
    strcpy(t->title,"Midnight");
}
int spmp_theme_load(spmp_theme *t,const char *path){
    FILE *f=fopen(path,"rb");if(!f)return -1;
    spmp_theme draft=*t;
    char line[256];int bad=0,count=0;
    while(fgets(line,sizeof line,f)){
        if(++count>128 || (!strchr(line,'\n') && !feof(f))){bad=1;break;}
        char *s=trim(line);
        if(!*s || *s=='#' || *s==';' || *s=='[')continue;
        char *eq=strchr(s,'=');if(!eq){bad=1;break;}
        *eq++=0;char *key=trim(s),*val=trim(eq);
        if(!strcmp(key,"title")){
            if(strlen(val)>31){bad=1;break;}strcpy(draft.title,val);continue;
        }
        uint16_t *dest=NULL;
        if(!strcmp(key,"background"))dest=&draft.background;
        else if(!strcmp(key,"panel"))dest=&draft.panel;
        else if(!strcmp(key,"accent"))dest=&draft.accent;
        else if(!strcmp(key,"text"))dest=&draft.text;
        else if(!strcmp(key,"muted"))dest=&draft.muted;
        else if(!strcmp(key,"selection"))dest=&draft.selection;
        else if(!strcmp(key,"border"))dest=&draft.border;
        else {bad=1;break;}
        if(color(val,dest)){bad=1;break;}
    }
    if(ferror(f))bad=1;
    fclose(f);
    if(bad)return -1;
    *t=draft;return 0;
}
