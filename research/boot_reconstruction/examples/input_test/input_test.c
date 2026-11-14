#include "input_test.h"
#include "font5x7.h"
const char input_log_path[]="/fat20a1/hda1/SPMPKEYS.CSV";
static const char *const prompts[14]={"UP","DOWN","LEFT","RIGHT","A","B","X","Y","L","R","START","SELECT","BACK","FREE"};
const char *input_prompt(uint32_t i){return prompts[i<13?i:13];}
static uint32_t length(const char *s){uint32_t n=0;while(s[n])n++;return n;}
static char *copy(char *p,const char *s){while(*s)*p++=*s++;return p;}
static char *hex32(char *p,uint32_t n){static const char h[]="0123456789ABCDEF";for(unsigned i=0;i<8;i++)*p++=h[(n>>(28-4*i))&15];return p;}
static int rect(const struct input_services *io,int x,int y,unsigned w,unsigned h)
{const struct mmi_rect_s16 r={(int16_t)x,(int16_t)y,(uint16_t)w,(uint16_t)h};return io->graphics.fill(&r)==0;}
static int color(const struct input_services *io,unsigned r,unsigned g,unsigned b)
{const struct mmi_rgb888 c={(uint8_t)r,(uint8_t)g,(uint8_t)b};return io->graphics.set_color(&c)==0;}
static const unsigned char *glyph(char c)
{if(c>='A'&&c<='Z')return input_letters[(unsigned)(c-'A')];if(c>='0'&&c<='9')return input_digits[(unsigned)(c-'0')];return 0;}
static int text(const struct input_services *io,int x,int y,const char *s)
{
 for(;*s;s++,x+=12){
  const unsigned char *g=glyph(*s);if(!g)continue;
  for(unsigned row=0;row<7;row++){
   unsigned col=0;
   while(col<5){
    if(!(g[row]&(16u>>col))){col++;continue;}
    unsigned start=col;while(col<5&&(g[row]&(16u>>col)))col++;
    if(!rect(io,x+(int)start*2,y+(int)row*2,(col-start)*2,2))return 0;
   }
  }
 }
 return 1;
}
static int line(const struct input_services *io,int y,const char *s)
{return color(io,0,0,0)&&rect(io,4,y,248,16)&&color(io,232,240,248)&&text(io,4,y,s);}
static int number_line(const struct input_services *io,int y,const char *label,uint32_t value)
{char b[32];char *p=copy(b,label);p=hex32(p,value);*p=0;return line(io,y,b);}
static const char *log_label(uint32_t n)
{static const char *const names[]={"SD LOG OK","SD OPEN ERROR","SD SEEK ERROR","SD WRITE ERROR","SD CLOSE ERROR","SD LOG LIMIT"};return names[n<6?n:3];}
static const char *log_caption(const struct input_state *s)
{return !s->log_status&&!s->log_records?"SD LOG CHECK":log_label(s->log_status);}
static void journal(struct input_state *s,const struct input_services *io,const char *row)
{
 if(s->log_status)return;
 uint32_t n=length(row);
 if(n>256||s->log_bytes>65536u-n){s->log_status=INPUT_LOG_LIMIT;return;}
 /* O_WRONLY|O_CREAT, no TRUNC. Seek to end explicitly before every record.
  * Absolute SD path only; never the native fsOpen prefix or internal fallback.
  */
 int32_t fd=io->open(input_log_path,0xau,0600u);
 if(fd<0){s->log_status=INPUT_LOG_OPEN_ERROR;return;}
 int32_t end=io->seek(fd,0,2);
 if(end<0)s->log_status=INPUT_LOG_SEEK_ERROR;
 else if((uint32_t)end>1048576u-n)s->log_status=INPUT_LOG_LIMIT;
 uint32_t written=0;
 for(unsigned attempt=0;!s->log_status&&written<n&&attempt<8;attempt++){
  int32_t count=io->write(fd,row+written,n-written);
  if(count<=0||(uint32_t)count>n-written){s->log_status=INPUT_LOG_WRITE_ERROR;break;}
  written+=(uint32_t)count;s->log_bytes+=(uint32_t)count;
 }
 if(!s->log_status&&written!=n)s->log_status=INPUT_LOG_WRITE_ERROR;
 if(io->close(fd)<0&&s->log_status==INPUT_LOG_OK)s->log_status=INPUT_LOG_CLOSE_ERROR;
 if(s->log_status==INPUT_LOG_OK)s->log_records++;
}
static void event(struct input_state *s,const struct input_services *io,const char *kind)
{
 char row[128];char *p=copy(row,kind);*p++=',';p=hex32(p,s->polls);*p++=',';
 p=hex32(p,s->elapsed);*p++=',';p=copy(p,input_prompt(s->prompt));*p++=',';
 p=hex32(p,s->keys);*p++=',';p=hex32(p,s->pressed);*p++=',';p=hex32(p,s->released);*p++='\n';*p=0;
 journal(s,io,row);
}
static int menu_ready(const struct input_state *s)
{return (s->sample_valid&0x13u)==0x13u&&s->sample[0]!=s->sample[1]&&s->sample[0]!=s->sample[4]&&s->sample[1]!=s->sample[4];}
static int learned_press(const struct input_state *s,unsigned i)
{return (s->sample_valid&(1u<<i))&&s->sample[i]&&(s->pressed&s->sample[i])==s->sample[i];}
static int screen(struct input_state *s,const struct input_services *io,int force)
{
 int dirty=force;int view_change=s->ui_view!=s->menu_view;int prompt_change=s->ui_prompt!=s->prompt;char label[32];
 if(force){if(!color(io,0,0,0)||!rect(io,0,0,320,240)||!color(io,232,240,248)||!text(io,4,4,"SPMP INPUT TEST"))return 0;}
 if(force||s->ui_prompt!=s->prompt||s->ui_menu!=s->menu_choice){
  char *p;
  if(s->prompt==13&&menu_ready(s)){static const char *const choices[]={"LIVE","MAP","EXIT"};p=copy(label,"MENU ");p=copy(p,choices[s->menu_choice]);}
  else {p=copy(label,"PRESS ");p=copy(p,input_prompt(s->prompt));}
  *p=0;if(!line(io,24,label))return 0;s->ui_prompt=s->prompt;s->ui_menu=s->menu_choice;dirty=1;
 }
 if(force||view_change||s->ui_keys!=s->keys||s->ui_events!=s->events){
  if(s->menu_view){
   if(!number_line(io,44,"UP ",s->sample[0])||!number_line(io,62,"DOWN ",s->sample[1])||
      !number_line(io,80,"LEFT ",s->sample[2])||!number_line(io,98,"RIGHT ",s->sample[3]))return 0;
  }else if(!number_line(io,44,"KEY ",s->keys)||!number_line(io,62,"NEW ",s->pressed)||
     !number_line(io,80,"OFF ",s->released)||!number_line(io,98,"LAST ",s->last))return 0;
  for(unsigned i=0;i<32;i++){
   int on=(s->keys>>i)&1u;
   if(!color(io,on?32:32,on?224:40,on?96:48)||!rect(io,4+(int)(i%16)*19,200+(int)(i/16)*18,15,12))return 0;
  }
  s->ui_keys=s->keys;s->ui_events=s->events;dirty=1;
 }
 if(force||s->ui_log!=s->log_status){if(!line(io,118,log_caption(s)))return 0;s->ui_log=s->log_status;dirty=1;}
 if(force||view_change||s->ui_second!=s->elapsed/1000u){
  if(!number_line(io,138,"TIME ",s->elapsed)||!number_line(io,156,s->menu_view?"A ":"ALL ",s->menu_view?s->sample[4]:s->seen))return 0;
  s->ui_second=s->elapsed/1000u;dirty=1;
 }
 if(force||prompt_change){if(!line(io,176,s->prompt==13&&menu_ready(s)?"UP DOWN A SELECT":"AUTO END 60S NOM"))return 0;dirty=1;}
 s->ui_view=s->menu_view;
 /* Cursor uses recorded direction samples when present; masks are candidates. */
 if(force||s->ui_cursor_tick!=s->elapsed/100u){
  uint32_t direction[4];for(unsigned i=0;i<4;i++)direction[i]=(s->sample_valid&(1u<<i))?s->sample[i]:(1u<<i);
  if((s->keys&direction[0])==direction[0])s->y-=3;
  if((s->keys&direction[1])==direction[1])s->y+=3;
  if((s->keys&direction[2])==direction[2])s->x-=3;
  if((s->keys&direction[3])==direction[3])s->x+=3;
  if(s->x<264)s->x=264;
  if(s->x>302)s->x=302;
  if(s->y<28)s->y=28;
  if(s->y>94)s->y=94;
  if(force||s->x!=s->ui_x||s->y!=s->ui_y){
   if(!color(io,0,0,0)||!rect(io,s->ui_x,s->ui_y,10,10)||!color(io,64,160,248)||!rect(io,s->x,s->y,10,10))return 0;
   s->ui_x=s->x;s->ui_y=s->y;dirty=1;
  }
  s->ui_cursor_tick=s->elapsed/100u;
 }
 if(dirty){if(io->graphics.paint())return 0;s->frames++;}
 return 1;
}
uint32_t input_test_run(struct input_state *s,const struct input_services *io,const struct input_config *c)
{
 if(!s||!io||!c||!c->duration_units||c->duration_units>60000||!c->max_polls||c->max_polls>3000||!c->prompt_units||c->prompt_units>60000||!c->redraw_units)return 2;
 if(!io->time||!io->delay||!io->open||!io->write||!io->seek||!io->close)return 2;
 /* Caller supplies zeroed state (startup BSS or host test initializer). */
 s->x=282;s->y=56;s->ui_x=282;s->ui_y=56;s->ui_keys=0xffffffffu;s->ui_prompt=0xffffffffu;
 s->ui_second=0xffffffffu;s->ui_log=0xffffffffu;s->ui_cursor_tick=0xffffffffu;
 struct probe_oem_adapter adapter={&io->graphics};
 if(probe_oem_operations()->begin(&adapter))return 2;
 uint16_t w=0,h=0;if(io->graphics.dimensions(&w,&h)||w!=320||h!=240)return 2;
 if(io->graphics.get_color(&s->original_color))return 2;
 s->color_saved=1;
 s->result=0;
 if(!screen(s,io,1)){s->result=2;goto done;}
 journal(s,io,"\n#SPMP_INPUT_TEST_V1,source=5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e,clock=MMI_TICK_DELTA_X10,labels=PROMPTS_NOT_AUTODETECTED\n");
 journal(s,io,"event,poll_hex,time_hex,prompt,mask_hex,pressed_hex,released_hex\n");
 s->start=io->time();
 if(!screen(s,io,1)||!line(io,24,"RELEASE BUTTONS")||io->graphics.paint()){s->result=2;goto done;}
 s->frames++;
 /* Do not mislabel a held launcher button as the requested UP key. */
 unsigned arm_polls=0;
 for(;arm_polls<150;arm_polls++){
  struct mmi_key_output32 initial={0,0};
  if(io->graphics.keys(&initial)!=1){s->result=2;goto done;}
  s->elapsed=io->time()-s->start;
  if(!initial.keys)break;
  if(initial.keys!=s->keys){s->pressed=initial.keys&~s->keys;s->released=s->keys&~initial.keys;s->keys=initial.keys;event(s,io,"ARM");}
  if(s->elapsed>=3000){s->result=2;goto done;}
  io->delay(2,0);
 }
 if(arm_polls==150){s->result=2;goto done;}
 uint32_t guide_offset=io->time()-s->start;
 s->keys=0;s->pressed=0;s->released=0;event(s,io,"PROMPT");
 if(!screen(s,io,1)){s->result=2;goto done;}
 uint32_t last_redraw=0;
 while(s->polls<c->max_polls){
  s->elapsed=io->time()-s->start;
  if(s->elapsed>=c->duration_units)break;
  uint32_t prompt=0;while(prompt<13&&s->elapsed-guide_offset>=c->prompt_units*(prompt+1u))prompt++;
  if(prompt!=s->prompt){s->prompt=prompt;s->pressed=0;s->released=0;event(s,io,"PROMPT");}
  struct mmi_key_output32 key={0,0};if(io->graphics.keys(&key)!=1){s->result=2;break;}
  uint32_t old=s->keys;s->keys=key.keys;s->seen|=s->keys;
  if(s->keys!=old){s->pressed=s->keys&~old;s->released=old&~s->keys;s->events++;if(s->keys)s->last=s->keys;event(s,io,"KEY");
   if(old==0&&s->keys&&s->prompt<13&&!(s->sample_valid&(1u<<s->prompt))){
    s->sample[s->prompt]=s->keys;s->sample_valid|=1u<<s->prompt;event(s,io,"SAMPLE");
   }
  }
  if(s->keys!=old&&s->prompt==13&&menu_ready(s)&&s->pressed){
   if(learned_press(s,0))s->menu_choice=s->menu_choice?s->menu_choice-1:2;
   else if(learned_press(s,1))s->menu_choice=s->menu_choice==2?0:s->menu_choice+1;
   else if(learned_press(s,4)){
    event(s,io,"MENU");if(s->menu_choice==2)break;s->menu_view=s->menu_choice;
   }
  }
  if(s->elapsed-last_redraw>=c->redraw_units||s->polls==0){
   if(!screen(s,io,0)){s->result=2;break;}last_redraw=s->elapsed;
  }
  s->polls++;io->delay(2,0);
 }
done:
 s->pressed=0;s->released=0;event(s,io,s->result?"END_ERROR":"END");
 if(!s->result){if(!line(io,24,"FINISHED")||!line(io,118,log_caption(s))||io->graphics.paint())s->result=2;else {s->frames++;io->delay(25,0);}}
 if(s->color_saved&&io->graphics.set_color(&s->original_color))s->result=2;
 return s->result;
}
