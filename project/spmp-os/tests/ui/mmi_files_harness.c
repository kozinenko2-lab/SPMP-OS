#include "spmp_mmi_files.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
struct fixture {char path[512];unsigned index,opened,closed;int fault;};
static int open_dir(void *v,const char *path){struct fixture *f=v;snprintf(f->path,sizeof f->path,"%s",path);f->index=0;f->opened++;return 1;}
static int next(void *v,int fd,char name[260]){
 struct fixture *f=v;assert(fd==1);
 if(f->fault==1)return -5;
 if(f->fault==2){strcpy(name,"../escape");return 1;}
 if(f->fault==3){snprintf(name,260,"%04u",f->index++);return 1;}
 const char *tail=strrchr(f->path,'/');tail=tail?tail+1:f->path;
 if(!strcmp(tail,"Ромы")){if(f->index++)return 0;strcpy(name,"Adventure renamed.a26");return 1;}
 if(!strcmp(tail,"User")){
  if(f->index==0){strcpy(name,"Ромы");f->index++;return 1;}
  if(f->index>23)return 0;
  snprintf(name,260,"F%03u.a26",23-f->index++);return 1;
 }
 if(f->index++)return 0;
 strcpy(name,!strcmp(tail,"System")?"Cores":"SPMP-OS");return 1;
}
static int stat_entry(void *v,const char *path,struct spmp_sd_meta *m){
 struct fixture *f=v;if(f->fault==4)return -2;
 const char *tail=strrchr(path,'/');assert(tail);m->mode=strstr(tail,".a26")?8:1;m->bytes=m->mode==8?4096:0;return 0;
}
static int close_dir(void *v,int fd){struct fixture *f=v;assert(fd==1);f->closed++;return f->fault==5?-9:0;}
static void press(struct spmp_mmi_files *f,unsigned key){struct spmp_mmi_frame in={0};in.pressed=SPMP_MMI_BUTTON(key);spmp_mmi_files_update(f,&in);}
int main(void){
 struct fixture fs={0};struct spmp_sd_io io={&fs,open_dir,next,stat_entry,close_dir};struct spmp_mmi_files f;
 assert(!spmp_mmi_files_init(&f,&io));assert(!strcmp(f.browser.panels[0].path,"SPMP-OS/User"));assert(f.browser.panels[0].total==24);
 assert(!strcmp(f.browser.panels[0].entries[0].name,"Ромы"));assert(!strcmp(f.browser.panels[0].entries[1].name,"F000.a26"));
 assert(!(spmp_mmi_fm_buttons(SPMP_MMI_BUTTON(SPMP_MMI_X))&FM01_KEY_START));
 assert(spmp_mmi_fm_buttons(SPMP_MMI_BUTTON(SPMP_MMI_START))==FM01_KEY_START);
 assert(spmp_mmi_fm_buttons(SPMP_MMI_BUTTON(SPMP_MMI_SELECT))==FM01_KEY_SELECT);
 press(&f,SPMP_MMI_R);assert(f.browser.active==1);press(&f,SPMP_MMI_L);assert(f.browser.active==0);
 press(&f,SPMP_MMI_A);assert(strstr(f.browser.panels[0].path,"/Ромы"));press(&f,SPMP_MMI_A);
 assert(f.details&&f.selected_atari&&!strcmp(f.selected_path,"SPMP-OS/User/Ромы/Adventure renamed.a26"));
 press(&f,SPMP_MMI_B);assert(!f.details);press(&f,SPMP_MMI_B);assert(!strcmp(f.browser.panels[0].path,"SPMP-OS/User"));
 for(unsigned n=0;n<10;n++)press(&f,SPMP_MMI_DOWN);
 assert(f.browser.panels[0].first==9&&!strcmp(fm01_selected(&f.browser,0)->name,"F009.a26"));
 for(unsigned n=0;n<10;n++)press(&f,SPMP_MMI_UP);
 assert(f.browser.panels[0].first==0&&!strcmp(fm01_selected(&f.browser,0)->name,"Ромы"));
 char out[512];assert(spmp_sd_path(&f.directory,"../escape",out,sizeof out));assert(spmp_sd_path(&f.directory,"a//b",out,sizeof out));assert(spmp_sd_path(&f.directory,"a\\b",out,sizeof out));
 assert(!spmp_sd_path(&f.directory,"SPMP-OS/User/Ромы/Adventure renamed.a26",out,sizeof out));assert(strstr(out,"/fat20a0/hda0/"));
 fm01_entry small[2];memset(small,0x5a,sizeof small);fm01_entry guard=small[1];
 uint32_t returned,total,files,dirs;
 assert(!spmp_sd_list_page(&f.directory,"SPMP-OS/User",0,1,small,&returned,&total,&files,&dirs));
 assert(returned==1&&total==24&&!strcmp(small[0].name,"Ромы"));assert(!memcmp(&small[1],&guard,sizeof guard));
 for(int fault=1;fault<=5;fault++){
  fs.fault=fault;spmp_sd_directory_invalidate(&f.directory);unsigned closed=fs.closed;
  assert(fm01_refresh(&f.browser,0));assert(fs.closed==closed+1);assert(!f.browser.panels[0].loaded);
 }
 assert(fs.opened==fs.closed);
 puts("PASS: FM integration, raw/legacy button adapter, panels, paged sorting/backtracking, UTF8/renamed ROM path, traversal/limits/errors and directory cleanup");return 0;
}
