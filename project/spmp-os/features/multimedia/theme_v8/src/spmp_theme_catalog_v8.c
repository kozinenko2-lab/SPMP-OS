#include "spmp_theme_catalog_v8.h"
static const char * const ids[SPMP_THEME_V8_COUNT]={
"beach_sunset","auto_muse","tennis_breeze","night_track","seaside_serpentine","showroom_crystal","ruby_lounge","violet_night","golden_pearl","midnight_blue_gt","crimson_drift","sunset_street","glamour_rose","alpine_coast","neon_racer","comic_nights","action_legends","scarlet_machine","blue_velocity","velvet_gold","glamour","nature","racing","comics","action"};
static const char * const cats[SPMP_THEME_V8_COUNT]={
"girls","girls","girls","cars","cars","cars","girls","girls","girls","cars","cars","cars","girls","nature","cars","comics","action","cars","cars","girls","girls","nature","cars","comics","action"};
static int eq(const char*a,const char*b){if(!a||!b)return 0;while(*a&&*b){if(*a++!=*b++)return 0;}return !*a&&!*b;}
size_t spmp_theme_v8_count(void){return SPMP_THEME_V8_COUNT;}
const char *spmp_theme_v8_id(size_t n){return n<SPMP_THEME_V8_COUNT?ids[n]:0;}
const char *spmp_theme_v8_category(size_t n){return n<SPMP_THEME_V8_COUNT?cats[n]:0;}
int spmp_theme_v8_find(const char *id){for(size_t i=0;i<SPMP_THEME_V8_COUNT;i++)if(eq(ids[i],id))return (int)i;return -1;}
int spmp_theme_v8_path(size_t n,const char *leaf,char*out,size_t cap){
 if(!out||n>=SPMP_THEME_V8_COUNT||!leaf)return -1;
 if(!eq(leaf,"background.spbg")&&!eq(leaf,"style.spth")&&!eq(leaf,"theme.json"))return -2;
 const char*parts[]={"themes/",ids[n],"/",leaf};size_t pos=0;
 for(unsigned k=0;k<4;k++)for(const char*p=parts[k];*p;p++){
  if(pos>=cap||pos+1>=cap){if(cap)out[0]=0;return -3;}
  out[pos++]=*p;
 }
 out[pos]=0;return (int)pos;
}
