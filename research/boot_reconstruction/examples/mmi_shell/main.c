#include "startup.h"
#include "input_test.h"
#include "font5x7.h"
#include "spmp_mmi_input.h"
#include "spmp_mmi_user01_profile.h"
#include "spmp_mmi_shell.h"
struct input_state spmp_shell_journal;
struct spmp_mmi_input spmp_shell_input;
struct spmp_mmi_frame spmp_shell_frame;
struct spmp_mmi_shell spmp_shell_model;
uint32_t spmp_shell_binding_status,spmp_shell_exit_reason;
struct shell_config {uint32_t duration_units,max_polls,redraw_units;};
struct shell_config spmp_shell_config={150000,7500,100};
_Static_assert(sizeof(struct spmp_mmi_frame)==40,"Frame trace layout");
_Static_assert(sizeof(struct spmp_mmi_shell)==24,"Menu trace layout");
void spmp_shell_snapshot(void *,struct spmp_mmi_snapshot *);
uint32_t spmp_shell_collect(void *);
uint32_t spmp_input_bind(uintptr_t,struct input_services *);
static const char path[]="/fat20a0/hda0/SPMPOS.CSV";
static char *cp(char *p,const char *s){while(*s)*p++=*s++;return p;}
static char *hx(char *p,uint32_t n){static const char h[]="0123456789ABCDEF";for(unsigned i=0;i<8;i++)*p++=h[(n>>(28-4*i))&15];return p;}
static char *field(char *p,uint32_t n){*p++=',';return hx(p,n);}
static void log_event(const struct input_services *io,const char *kind)
{
 struct input_state *s=&spmp_shell_journal;const struct spmp_mmi_frame *f=&spmp_shell_frame;
 if(s->log_status)return;
 char row[192];char *p=cp(row,kind);p=field(p,s->polls);p=field(p,s->elapsed);p=field(p,spmp_shell_model.view);
 p=field(p,spmp_shell_model.selection);p=field(p,f->state);p=field(p,f->buttons);p=field(p,f->power_short);p=field(p,f->blocked);p=field(p,f->chords);*p++='\n';*p=0;
 input_journal_append(s,io,path,row);
}
static int color(const struct input_services *io,unsigned r,unsigned g,unsigned b)
{const struct mmi_rgb888 c={(uint8_t)r,(uint8_t)g,(uint8_t)b};return !io->graphics.set_color(&c);}
static int rect(const struct input_services *io,int x,int y,unsigned w,unsigned h)
{const struct mmi_rect_s16 r={(int16_t)x,(int16_t)y,(uint16_t)w,(uint16_t)h};return !io->graphics.fill(&r);}
static int text(const struct input_services *io,int x,int y,const char *str)
{
 for(;*str;str++,x+=12){
  const unsigned char *glyph=*str>='A'&&*str<='Z'?input_letters[(unsigned)(*str-'A')]:*str>='0'&&*str<='9'?input_digits[(unsigned)(*str-'0')]:0;
  if(!glyph)continue;
  for(unsigned yy=0;yy<7;yy++){
   unsigned xx=0;
   while(xx<5){if(!(glyph[yy]&(16u>>xx))){xx++;continue;}unsigned begin=xx;while(xx<5&&(glyph[yy]&(16u>>xx)))xx++;
    if(!rect(io,x+(int)begin*2,y+(int)yy*2,(xx-begin)*2,2))return 0;
   }
  }
 }
 return 1;
}
static int line(const struct input_services *io,int y,const char *str)
{return color(io,10,16,28)&&rect(io,8,y,304,18)&&color(io,230,238,250)&&text(io,10,y,str);}
static int number(const struct input_services *io,int y,const char *label,uint32_t n)
{char b[32];char *p=cp(b,label);p=hx(p,n);*p=0;return line(io,y,b);}
static int item(const struct input_services *io,int y,const char *label,int selected)
{return color(io,selected?38:18,selected?90:30,selected?146:48)&&rect(io,8,y-8,304,32)&&color(io,selected?255:190,selected?255:205,selected?255:225)&&text(io,20,y,label);}
static int draw(const struct input_services *io,int final)
{
 const struct spmp_mmi_shell *m=&spmp_shell_model;const struct spmp_mmi_frame *f=&spmp_shell_frame;struct input_state *s=&spmp_shell_journal;
 if(!color(io,10,16,28)||!rect(io,0,0,320,240)||!color(io,90,190,250)||!text(io,10,8,"SPMP OS MINI"))return 0;
 if(final){if(!line(io,72,"RETURNING TO MMI"))return 0;}
 else if(m->view==SPMP_MMI_HOME){
  if(!line(io,30,"RAM SHELL 01")||!item(io,64,"INFORMATION",m->selection==0)||!item(io,108,"CONTROLS",m->selection==1)||!item(io,152,"EXIT",m->selection==2))return 0;
 }else if(m->view==SPMP_MMI_INFO){
  if(!line(io,40,"INFORMATION")||!line(io,68,"MMI HOSTED")||!line(io,94,"PROFILE USER01")||!number(io,120,"TIME ",s->elapsed)||!line(io,150,"LOG SPMPOS CSV"))return 0;
 }else if(m->view==SPMP_MMI_CONTROLS){
  if(!line(io,40,"CONTROLS")||!number(io,66,"STATE ",f->state)||!number(io,92,"KEYS ",f->buttons)||!number(io,118,"OTHER ",f->unmapped_state)||!number(io,144,"CHORD ",f->chords)||!line(io,172,"POWER OPENS MENU"))return 0;
 }else if(m->view==SPMP_MMI_POWER_MENU){
  if(!line(io,40,"APPLICATION MENU")||!item(io,80,"CONTINUE",m->power_selection==0)||!item(io,126,"EXIT",m->power_selection==1))return 0;
 }
 if(s->log_status>=1&&s->log_status<=4){
  static const char *const errors[]={"","SD OPEN E ","SD SEEK E ","SD WRITE E ","SD CLOSE E "};
  if(!number(io,198,errors[s->log_status],s->log_errno))return 0;
 }else if(!line(io,198,s->log_status?"SD LOG LIMIT":s->log_records?"SD LOG OK":"SD LOG CHECK"))return 0;
 if(!line(io,220,f->blocked?"INPUT LOCKED":!f->armed?"RELEASE BUTTONS":m->view==SPMP_MMI_HOME?"A OPEN B EXIT":"A SELECT B BACK")||io->graphics.paint())return 0;
 s->frames++;return 1;
}
uint32_t spmp_app_main(uintptr_t raw)
{
 struct input_services io;spmp_shell_binding_status=spmp_input_bind(raw,&io);
 if(spmp_shell_binding_status)return spmp_shell_binding_status;
 if(!spmp_shell_config.duration_units||spmp_shell_config.duration_units>150000||!spmp_shell_config.max_polls||spmp_shell_config.max_polls>7500||!spmp_shell_config.redraw_units)return 2;
 struct spmp_mmi_io input;input.user=0;input.read=spmp_shell_snapshot;input.collect=spmp_shell_collect;
 if(spmp_mmi_input_init(&spmp_shell_input,&spmp_mmi_user01_profile,&input))return 2;
 spmp_mmi_shell_init(&spmp_shell_model);struct input_state *s=&spmp_shell_journal;
 struct probe_oem_adapter adapter={&io.graphics};if(probe_oem_operations()->begin(&adapter))return 2;
 uint16_t w=0,h=0;if(io.graphics.dimensions(&w,&h)||w!=320||h!=240||io.graphics.get_color(&s->original_color))return 2;
 s->color_saved=1;s->result=0;
 if(!draw(&io,0)){s->result=2;goto done;}
 input_journal_append(s,&io,path,"\n#SPMP_OS_MINI_V1,profile=DEFENDER_USER01,source=5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e,mode=MMI_RAM\n");
 input_journal_append(s,&io,path,"event,poll_hex,time_hex,view_hex,selection_hex,state_hex,buttons_hex,power_hex,blocked_hex,chords_hex\n");
 s->start=io.time();log_event(&io,"START");
 uint32_t last_draw=0,last_armed=0,last_log=0,last_state=0,last_second=0;int dirty=1;
 while(s->polls<spmp_shell_config.max_polls){
  s->elapsed=io.time()-s->start;
  if(s->elapsed>=spmp_shell_config.duration_units){spmp_shell_exit_reason=2;break;}
  if(spmp_mmi_input_poll(&spmp_shell_input,&spmp_shell_frame)){s->result=2;break;}
  struct spmp_mmi_frame *f=&spmp_shell_frame;int changed=spmp_mmi_shell_update(&spmp_shell_model,f);
  if(changed||f->pressed||f->released||f->power_short||f->chords){s->events++;log_event(&io,f->power_short?"POWER":f->chords?"CHORD":"INPUT");}
  if(changed||last_armed!=f->armed||last_log!=s->log_status)dirty=1;
  if(spmp_shell_model.view==SPMP_MMI_CONTROLS&&f->state!=last_state&&!f->blocked)dirty=1;
  if(spmp_shell_model.view==SPMP_MMI_INFO&&s->elapsed/1000u!=last_second)dirty=1;
  last_state=f->state;last_second=s->elapsed/1000u;last_armed=f->armed;last_log=s->log_status;
  if(spmp_shell_model.view==SPMP_MMI_EXIT){spmp_shell_exit_reason=1;break;}
  if(dirty&&s->elapsed-last_draw>=spmp_shell_config.redraw_units){if(!draw(&io,0)){s->result=2;break;}last_draw=s->elapsed;dirty=0;}
  s->polls++;io.delay(2,0);
 }
 if(!spmp_shell_exit_reason)spmp_shell_exit_reason=3;
 done:
 log_event(&io,s->result?"END_ERROR":"END");
 if(!s->result){if(!draw(&io,1))s->result=2;else io.delay(25,0);}
 if(s->color_saved&&io.graphics.set_color(&s->original_color))s->result=2;
 return s->result;
}
