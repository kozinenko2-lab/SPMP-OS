#include "startup.h"
#include "input_test.h"
#include "spmp_mmi_input.h"
#include "spmp_mmi_user01_profile.h"
#include "spmp_mmi_shell.h"
#include "spmp_mmi_files.h"
#include "display.h"
#define FILES_VIEW 5u
struct input_state spmp_shell_journal;
struct spmp_mmi_input spmp_shell_input;
struct spmp_mmi_frame spmp_shell_frame;
struct spmp_mmi_shell spmp_shell_model;
struct spmp_mmi_files spmp_files;
uint32_t spmp_shell_binding_status,spmp_shell_exit_reason;
struct shell_config {uint32_t duration_units,max_polls,redraw_units;};
struct shell_config spmp_shell_config={150000,7500,100};
void spmp_shell_snapshot(void *,struct spmp_mmi_snapshot *);
uint32_t spmp_shell_collect(void *);
uint32_t spmp_input_bind(uintptr_t,struct input_services *);
void spmp_files_bind(struct spmp_sd_io *);
static const char path[]="/fat20a0/hda0/SPMP-OS/User/Logs/SPMPFM.CSV";
static char *cp(char *p,const char *s){while(*s)*p++=*s++;return p;}
static char *hx(char *p,uint32_t n){static const char h[]="0123456789ABCDEF";for(unsigned i=0;i<8;i++)*p++=h[(n>>(28-4*i))&15];return p;}
static char *field(char *p,uint32_t n){*p++=',';return hx(p,n);}
static void log_event(const struct input_services *io,const char *kind){
 struct input_state *s=&spmp_shell_journal;const struct spmp_mmi_frame *f=&spmp_shell_frame;if(s->log_status)return;
 char row[192];char *p=cp(row,kind);p=field(p,s->polls);p=field(p,s->elapsed);p=field(p,spmp_shell_model.view);
 uint32_t selection=spmp_shell_model.view==FILES_VIEW?((uint32_t)spmp_files.browser.active<<31)|spmp_files.browser.panels[spmp_files.browser.active].cursor:spmp_shell_model.view==SPMP_MMI_POWER_MENU?spmp_shell_model.power_selection:spmp_shell_model.selection;
 p=field(p,selection);p=field(p,f->state);p=field(p,f->buttons);p=field(p,f->power_short);p=field(p,f->blocked);p=field(p,f->chords);*p++='\n';*p=0;input_journal_append(s,io,path,row);
}
static void log_path(const struct input_services *io){
 static const char hex[]="0123456789ABCDEF";const uint8_t *s=(const uint8_t*)spmp_files.selected_path;unsigned part=0;
 while(*s&&!spmp_shell_journal.log_status){
  char row[224];char *p=cp(row,"#PATH,");p=hx(p,part++);*p++=',';
  while(*s&&p<row+210){unsigned c=*s++;if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='/'||c=='.'||c=='-'||c=='_')*p++=(char)c;else{*p++='%';*p++=hex[c>>4];*p++=hex[c&15];}}
  *p++='\n';*p=0;input_journal_append(&spmp_shell_journal,io,path,row);
 }
}
static int text(const struct input_services *io,unsigned x,unsigned y,const char *s,unsigned width,uint32_t rgb){return spmp_files_text((void*)io,x,y,s,width,rgb);}
static int draw(const struct input_services *io,int final){
 const struct spmp_mmi_shell *m=&spmp_shell_model;int result=0;
 if(m->view==FILES_VIEW&&!final){struct spmp_file_draw ops={(void*)io,spmp_files_rect,spmp_files_text};result=spmp_mmi_files_draw(&spmp_files,&ops);}
 else{
  result=spmp_files_rect((void*)io,0,0,320,240,0x0a101c)||text(io,8,6,"SPMP-OS",304,0x70c8ff);
  if(final)result|=text(io,8,75,"Returning to MMI",304,0xffffff);
  else if(m->view==SPMP_MMI_HOME||m->view==SPMP_MMI_POWER_MENU){
   static const char *const home[]={"Files","Information","Exit"},*const power[]={"Continue","Exit"};
   unsigned count=m->view==SPMP_MMI_HOME?3:2,sel=m->view==SPMP_MMI_HOME?m->selection:m->power_selection;
   const char *const *labels=m->view==SPMP_MMI_HOME?home:power;
   for(unsigned i=0;i<count;i++){unsigned y=56+i*43;result|=spmp_files_rect((void*)io,6,y,308,34,i==sel?0x245a92:0x122032)||text(io,16,y+6,labels[i],295,0xffffff);}
   result|=text(io,6,220,"A Select  B Back",308,0xc5d9ef);
  }else{
   result|=text(io,6,44,"Files: two SD panels",308,0xffffff)||text(io,6,70,"System: apps and libraries",308,0xc5d9ef)||text(io,6,96,"User: ROMs and multimedia",308,0xc5d9ef)||text(io,6,122,"Atari ROM paths supported",308,0xc5d9ef)||text(io,6,150,"Atari player: not installed",308,0xc5d9ef)||text(io,6,176,spmp_files_font_mode?"Font: Roboto TTF":"Font: built-in fallback",308,0xc5d9ef)||text(io,6,220,"B Back",308,0xffffff);
  }
 }
 if(spmp_shell_frame.blocked)result|=text(io,230,2,"LOCKED",85,0xff8080);
 else if(spmp_shell_journal.log_status){
  static const char *const errors[]={"OK ","OPEN ","SEEK ","WRITE ","CLOSE ","LIMIT "};
  char label[24];char *p=cp(label,errors[spmp_shell_journal.log_status<=5?spmp_shell_journal.log_status:5]);p=hx(p,spmp_shell_journal.log_errno);*p=0;
  result|=text(io,150,2,label,165,0xff8080);
 }else if(!spmp_shell_frame.armed&&!final)result|=text(io,179,2,"Release keys",136,0xffc080);
 if(result||io->graphics.paint())return -1;
 spmp_shell_journal.frames++;return 0;
}
static void update(void){
 struct spmp_mmi_frame *f=&spmp_shell_frame;struct spmp_mmi_shell *m=&spmp_shell_model;
 if(f->blocked||f->power_short){spmp_mmi_shell_update(m,f);return;}
 if(m->view==FILES_VIEW){if(spmp_mmi_files_update(&spmp_files,f))m->view=SPMP_MMI_HOME;return;}
 if(m->view==SPMP_MMI_HOME&&(f->pressed&SPMP_MMI_BUTTON(SPMP_MMI_A))){m->view=m->selection==0?FILES_VIEW:m->selection==1?SPMP_MMI_INFO:SPMP_MMI_EXIT;return;}
 spmp_mmi_shell_update(m,f);
}
uint32_t spmp_app_main(uintptr_t raw){
 struct input_services io;spmp_shell_binding_status=spmp_input_bind(raw,&io);if(spmp_shell_binding_status)return spmp_shell_binding_status;
 struct input_state *s=&spmp_shell_journal;
 if(!spmp_shell_config.duration_units||spmp_shell_config.duration_units>150000||!spmp_shell_config.max_polls||spmp_shell_config.max_polls>7500||!spmp_shell_config.redraw_units)return 2;
 struct spmp_mmi_io input={0,spmp_shell_snapshot,spmp_shell_collect};if(spmp_mmi_input_init(&spmp_shell_input,&spmp_mmi_user01_profile,&input))return 2;
 spmp_mmi_shell_init(&spmp_shell_model);struct probe_oem_adapter adapter={&io.graphics};if(probe_oem_operations()->begin(&adapter))return 2;
 uint16_t w=0,h=0;if(io.graphics.dimensions(&w,&h)||w!=320||h!=240||io.graphics.get_color(&s->original_color))return 2;
 s->color_saved=1;
 spmp_files_font_open(&io);
 if(spmp_files_rect(&io,0,0,320,240,0x0a101c)||text(&io,8,65,"Reading SD directories...",304,0xffffff)||io.graphics.paint()){s->result=2;goto done;}
 struct spmp_sd_io filesystem;spmp_files_bind(&filesystem);if(spmp_mmi_files_init(&spmp_files,&filesystem)){s->result=2;goto done;}
 spmp_shell_model.view=FILES_VIEW;
 input_journal_append(s,&io,path,"\n#SPMP_OS_FILES_V1,profile=DEFENDER_USER01,mode=MMI_RAM,filesystem=SD0_READ_ONLY\n");
 input_journal_append(s,&io,path,"event,poll_hex,time_hex,view_hex,selection_hex,state_hex,buttons_hex,power_hex,blocked_hex,chords_hex\n");
 input_journal_append(s,&io,path,spmp_files_font_mode?"#FONT,Roboto-Regular.ttf,direct_TTF\n":"#FONT,built-in_bitmap,fallback\n");
 s->start=io.time();log_event(&io,"START");
 if(draw(&io,0)){s->result=2;goto done;}
 uint32_t last_draw=0,last_armed=0,last_view=FILES_VIEW,last_selection=0,last_change=0,last_block=0;int dirty=1;
 while(s->polls<spmp_shell_config.max_polls){
  s->elapsed=io.time()-s->start;if(s->elapsed>=spmp_shell_config.duration_units){spmp_shell_exit_reason=2;break;}
  if(spmp_mmi_input_poll(&spmp_shell_input,&spmp_shell_frame)){s->result=2;break;}
  uint32_t details=spmp_files.details;update();struct spmp_mmi_frame *f=&spmp_shell_frame;struct spmp_mmi_shell *m=&spmp_shell_model;
  if(f->pressed||f->released||f->power_short||f->chords){s->events++;log_event(&io,f->power_short?"POWER":f->chords?"CHORD":"INPUT");}
  if(!details&&spmp_files.details)log_path(&io);
  uint32_t selection=m->view==SPMP_MMI_POWER_MENU?m->power_selection:m->selection;
  if(last_armed!=f->armed||last_view!=m->view||last_selection!=selection||last_change!=spmp_files.changes||last_block!=f->blocked)dirty=1;
  last_armed=f->armed;last_view=m->view;last_selection=selection;last_change=spmp_files.changes;last_block=f->blocked;
  if(m->view==SPMP_MMI_EXIT){spmp_shell_exit_reason=1;break;}
  if(dirty&&s->elapsed-last_draw>=spmp_shell_config.redraw_units){if(draw(&io,0)){s->result=2;break;}last_draw=s->elapsed;dirty=0;}
  s->polls++;io.delay(2,0);
 }
 if(!spmp_shell_exit_reason)spmp_shell_exit_reason=3;
 done:
 log_event(&io,s->result?"END_ERROR":"END");
 if(!s->result){if(draw(&io,1))s->result=2;else io.delay(25,0);}
 if(s->color_saved&&io.graphics.set_color(&s->original_color))s->result=2;
 spmp_files_font_close();
 return s->result;
}
