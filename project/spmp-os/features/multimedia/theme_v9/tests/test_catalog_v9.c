#include "spmp_theme_catalog_v9.h"
#include <assert.h>
#include <string.h>
int main(void){char p[128];assert(spmp_theme_v9_count()==34);
 for(size_t i=0;i<spmp_theme_v9_count();i++){
  const char*id=spmp_theme_v9_id(i);assert(id&&spmp_theme_v9_category(i));
  assert(spmp_theme_v9_find(id)==(int)i);
  assert(spmp_theme_v9_path(i,"background.spbg",p,sizeof p)>0);
  assert(!strncmp(p,"themes/",7));}
 assert(spmp_theme_v9_find("../../dev/null")==-1);
 assert(spmp_theme_v9_path(999,"background.spbg",p,sizeof p)<0);
 assert(spmp_theme_v9_path(0,"../../etc/passwd",p,sizeof p)<0);
 assert(spmp_theme_v9_path(0,"background.spbg",p,8)<0);
 return 0;}
