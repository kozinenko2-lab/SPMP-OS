#include "spmp_mmi_files.h"
uint32_t spmp_mmi_fm_buttons(uint32_t keys){
 uint32_t out=keys&63u;
 if(keys&SPMP_MMI_BUTTON(SPMP_MMI_START))out|=FM01_KEY_START;
 if(keys&SPMP_MMI_BUTTON(SPMP_MMI_SELECT))out|=FM01_KEY_SELECT;
 if(keys&SPMP_MMI_BUTTON(SPMP_MMI_L))out|=FM01_KEY_LEFT;
 if(keys&SPMP_MMI_BUTTON(SPMP_MMI_R))out|=FM01_KEY_RIGHT;
 return out;
}
static void cp(char *out,const char *in){while((*out++=*in++)){} }
int spmp_mmi_files_init(struct spmp_mmi_files *f,const struct spmp_sd_io *io){
 if(!f||spmp_sd_directory_init(&f->directory,io,"/fat20a0/hda0"))return -1;
 f->selected_path[0]=0;f->details=f->selected_atari=f->changes=0;
 fm01_init(&f->browser,spmp_sd_list_page,&f->directory);f->browser.locale=1;
 static const char *const roots[]={"SPMP-OS/User","SPMP-OS/System"};
 for(unsigned i=0;i<2;i++){
  cp(f->browser.panels[i].path,roots[i]);f->browser.panels[i].depth=2;
  if(fm01_refresh(&f->browser,i)){
   f->browser.panels[i].path[0]=0;f->browser.panels[i].depth=0;fm01_refresh(&f->browser,i);
  }
 }
 return 0;
}
static int atari_name(const char *s){
 const char *dot=0;for(;*s;s++)if(*s=='.')dot=s;
 return dot&&(dot[1]=='a'||dot[1]=='A')&&dot[2]=='2'&&dot[3]=='6'&&dot[4]==0;
}
int spmp_mmi_files_update(struct spmp_mmi_files *f,const struct spmp_mmi_frame *in){
 if(!f||!in||in->blocked||in->power_short)return 0;
 uint32_t keys=spmp_mmi_fm_buttons(in->pressed);if(!keys&&!(in->pressed&SPMP_MMI_BUTTON(SPMP_MMI_X)))return 0;
 if(f->details){if(keys&FM01_KEY_B){f->details=0;f->changes++;}return 0;}
 if(keys&FM01_KEY_START)return 1;
 if((keys&FM01_KEY_B)&&!f->browser.panels[f->browser.active].depth)return 1;
 if(in->pressed&SPMP_MMI_BUTTON(SPMP_MMI_X)){
  spmp_sd_directory_invalidate(&f->directory);fm01_refresh(&f->browser,f->browser.active);f->changes++;return 0;
 }
 if(keys&FM01_KEY_A){
  const fm01_entry *e=fm01_selected(&f->browser,f->browser.active);
  if(e&&e->type==FM01_ENTRY_FILE){
   const char *path=f->browser.panels[f->browser.active].path;size_t n=0;
   while(path[n]){f->selected_path[n]=path[n];n++;}if(n)f->selected_path[n++]='/';
   size_t k=0;while(e->name[k]&&n+1<sizeof f->selected_path)f->selected_path[n++]=e->name[k++];
   if(e->name[k])return 0;
   f->selected_path[n]=0;
   f->selected_atari=(uint32_t)atari_name(e->name);f->details=1;f->changes++;return 0;
  }
 }
 fm01_buttons(&f->browser,keys);f->changes++;return 0;
}
static int t(const struct spmp_file_draw *d,unsigned x,unsigned y,const char *s,unsigned w,uint32_t c){return d->text(d->user,x,y,s,w,c);}
int spmp_mmi_files_draw(const struct spmp_mmi_files *f,const struct spmp_file_draw *d){
 if(!f||!d||!d->rect||!d->text)return -1;
 if(d->rect(d->user,0,0,320,240,0x0a101c)||t(d,5,2,"SPMP-OS FILES",235,0x70c8ff))return -1;
 if(f->details){
  if(t(d,6,30,f->selected_atari?"ATARI 2600 ROM":"FILE INFORMATION",308,0xffffff))return -1;
  const char *s=f->selected_path;char part[31];unsigned line=0;
  while(*s&&line<4){
   unsigned n=0;while(s[n]&&n<30)n++;
   if(s[n])while(n&&((uint8_t)s[n]&0xc0u)==0x80u)n--;
   if(!n)n=1;
   for(unsigned k=0;k<n;k++)part[k]=s[k];
   part[n]=0;s+=n;
   if(t(d,6,58+line*18,part,308,0xdde8fa))return -1;
   line++;
  }
  if(t(d,6,150,f->selected_atari?"External ROM path selected":"Read-only browser",308,0x8db8dd)||t(d,6,172,f->selected_atari?"Atari player not installed":"POWER opens application menu",308,0x8db8dd)||t(d,6,219,"B Back",308,0xffffff))return -1;
  return 0;
 }
 for(unsigned i=0;i<2;i++){
  unsigned x=i*160;const fm01_panel *p=&f->browser.panels[i];uint32_t border=i==f->browser.active?0x55d8ff:0x30465a;
  if(d->rect(d->user,x,22,159,1,border)||d->rect(d->user,x,22,1,180,border)||d->rect(d->user,x+158,22,1,180,border)||d->rect(d->user,x,201,159,1,border))return -1;
  const char *tail=p->path;for(const char *s=p->path;*s;s++)if(*s=='/')tail=s+1;
  if(t(d,x+4,25,*tail?tail:"SD root",150,0xdde8fa))return -1;
  if(p->last_error){
   char err[16]="Read E ";static const char hex[]="0123456789ABCDEF";uint32_t code=0u-(uint32_t)p->last_error;
   for(unsigned k=0;k<8;k++)err[7+k]=hex[(code>>(28-k*4))&15u];
   err[15]=0;if(t(d,x+4,80,err,150,0xff8080))return -1;
  }
  else if(!p->total){if(t(d,x+4,80,"Empty",150,0x9faabb))return -1;}
  for(unsigned j=0;j<p->loaded;j++){
   unsigned y=44+j*17;const fm01_entry *e=&p->entries[j];
   if(p->cursor==p->first+j&&i==f->browser.active&&d->rect(d->user,x+2,y,155,17,0x245a92))return -1;
   if(t(d,x+4,y,e->type==FM01_ENTRY_DIR?">":" ",10,0x72c6ff)||t(d,x+16,y,e->name,139,0xe6efff))return -1;
  }
 }
 if(t(d,5,202,"A Open  B Parent  X Refresh",310,0xc5d9ef)||t(d,5,220,"L/R Panel  START Home",310,0xc5d9ef))return -1;
 return 0;
}
