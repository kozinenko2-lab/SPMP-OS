#include "spmp_theme_catalog_v8.h"
#include <assert.h>
#include <string.h>
int main(void){char p[128];assert(spmp_theme_v8_count()==25);
 for(size_t i=0;i<spmp_theme_v8_count();i++){
  const char*id=spmp_theme_v8_id(i);assert(id&&spmp_theme_v8_category(i));
  assert(spmp_theme_v8_find(id)==(int)i);
  assert(spmp_theme_v8_path(i,"background.spbg",p,sizeof p)>0);
  assert(!strncmp(p,"themes/",7));}
 assert(spmp_theme_v8_find("../../dev/null")==-1);
 assert(spmp_theme_v8_path(999,"background.spbg",p,sizeof p)<0);
 assert(spmp_theme_v8_path(0,"../../etc/passwd",p,sizeof p)<0);
 assert(spmp_theme_v8_path(0,"background.spbg",p,8)<0);
 return 0;}
