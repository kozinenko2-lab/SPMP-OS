#include "spmp_buttons.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
static const char *actions[SPMP_ACTIONS]={"up","down","left","right","confirm","back","menu","select","next","previous","volume_up","volume_down"};
static const char *defaults[SPMP_ACTIONS]={"UP","DOWN","LEFT","RIGHT","A","B","POWER","SELECT","R","L","VOL_PLUS","VOL_MINUS"};
const char *spmp_button_action(size_t i){return i<SPMP_ACTIONS?actions[i]:NULL;}
void spmp_button_map_default(spmp_button_map *m){for(int i=0;i<SPMP_ACTIONS;i++)strcpy(m->mapping[i],defaults[i]);}
static char *strip(char *s){while(isspace((unsigned char)*s))s++;size_t n=strlen(s);while(n&&isspace((unsigned char)s[n-1]))s[--n]=0;return s;}
int spmp_button_map_load(spmp_button_map *m,const char *path){
 FILE *f=fopen(path,"rb");if(!f)return -1;
 spmp_button_map draft=*m;char b[256];int bad=0,lines=0;
 while(fgets(b,sizeof b,f)){
  if(++lines>128 || (!strchr(b,'\n')&&!feof(f))){bad=1;break;}
  char *p=strip(b);if(!*p||*p=='#'||*p==';'||*p=='[')continue;
  char *sep=strchr(p,'=');if(!sep){bad=1;break;}*sep++=0;
  char *key=strip(p),*v=strip(sep);
  int idx=-1;for(int i=0;i<SPMP_ACTIONS;i++)if(!strcmp(key,actions[i]))idx=i;
  if(idx<0||strlen(v)==0||strlen(v)>31){bad=1;break;}
  for(char *t=v;*t;t++){if(!(isalnum((unsigned char)*t)||*t=='_'||*t=='+')){bad=1;break;}}
  if(bad)break;
  strcpy(draft.mapping[idx],v);
 }
 if(ferror(f))bad=1;
 fclose(f);
 if(bad)return -1;
 *m=draft;return 0;
}
