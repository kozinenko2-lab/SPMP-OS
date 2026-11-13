#include "common.h"
static int eq(const char *a,const char *b){
 while(*a&&*b){if(*a++!=*b++)return 0;}return !*a&&!*b;
}
static char *strip(char *s){
 while(*s==' '||*s=='\t'||*s=='\r')s++;
 size_t n=0;while(s[n])n++;
 while(n&&(s[n-1]==' '||s[n-1]=='\t'||s[n-1]=='\r'))s[--n]=0;
 return s;
}
static int hexval(char c){
 if(c>='0'&&c<='9')return c-'0';
 if(c>='a'&&c<='f')return c-'a'+10;
 if(c>='A'&&c<='F')return c-'A'+10;
 return -1;
}
static int color(const char *s,uint16_t *out){
 if(s[0]!='#')return -1;
 unsigned c=0;
 for(unsigned i=1;i<7;i++){
  int d=hexval(s[i]);if(d<0)return -1;c=(c<<4)|(unsigned)d;
 }
 if(s[7])return -1;
 *out=mm_rgb565((c>>16)&255u,(c>>8)&255u,c&255u);
 return 0;
}
void mm_skin_default(mm_skin *t){
 if(!t)return;
 mm_zero(t,sizeof(*t));
 t->bg=0x1083;t->panel=0x2108;t->panel_alt=0x294a;
 t->accent=0x05ba;t->accent_alt=0x7bd7;t->text=0xffff;
 t->muted=0x9cf3;t->border=0x4208;
 t->active=0x334d;t->highlight=0x5af0;
 {const char *s="Midnight";unsigned i=0;while(s[i]){t->name[i]=s[i];i++;}}
}
int mm_skin_parse(mm_skin *skin,const char *input,size_t size){
 mm_skin draft;size_t p=0;unsigned nlines=0;
 if(!skin||!input||size>4096)return -1;
 draft=*skin;
 while(p<size){
  char tmp[128];size_t n=0;
  if(++nlines>96)return -1;
  while(p<size&&input[p]!='\n'){
   if(n>=sizeof(tmp)-1||input[p]==0)return -1;
   tmp[n++]=input[p++];
  }
  if(p<size)p++;
  tmp[n]=0;char *line=strip(tmp);
  if(!*line||*line==';'||*line=='#')continue;
  if(*line=='['){if(!eq(line,"[SPMP-MM]"))return -1;continue;}
  char *delim=line;while(*delim&&*delim!='=')delim++;
  if(!*delim)return -1;
  *delim++=0;
  char *key=strip(line),*value=strip(delim);
  if(eq(key,"format")){if(!eq(value,"1"))return -1;continue;}
  if(eq(key,"name")){
   size_t i=0;while(value[i]&&i<sizeof(draft.name))i++;
   if(i==sizeof(draft.name))return -1;
   mm_zero(draft.name,sizeof(draft.name));
   for(size_t j=0;j<i;j++)draft.name[j]=value[j];
   continue;
  }
  uint16_t *where=0;
  if(eq(key,"background"))where=&draft.bg;
  else if(eq(key,"panel"))where=&draft.panel;
  else if(eq(key,"panel_alt"))where=&draft.panel_alt;
  else if(eq(key,"accent"))where=&draft.accent;
  else if(eq(key,"accent_alt"))where=&draft.accent_alt;
  else if(eq(key,"text"))where=&draft.text;
  else if(eq(key,"muted"))where=&draft.muted;
  else if(eq(key,"border"))where=&draft.border;
  else if(eq(key,"active"))where=&draft.active;
  else if(eq(key,"highlight"))where=&draft.highlight;
  else return -1;
  if(color(value,where))return -1;
 }
 *skin=draft;return 0;
}
