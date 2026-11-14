#include "raw_input.h"
#include "font5x7.h"
const char raw_log_path[]="/fat20a0/hda0/SPMPRAW.CSV";
static const char *const names[]={"UP","DOWN","LEFT","RIGHT","A","B","X","Y","L","R","START","SELECT","HOLD_SHORT","VOL_PLUS","VOL_MINUS","SELECT_VOL_PLUS","SELECT_VOL_MINUS","HOLD_LOCK","HOLD_UNLOCK","POWER_SHORT","FREE"};
static const char *const labels[]={"PRESS UP","PRESS DOWN","PRESS LEFT","PRESS RIGHT","PRESS A","PRESS B","PRESS X","PRESS Y","PRESS L","PRESS R","PRESS START","PRESS SELECT","HOLD SHORT","PRESS VOL PLUS","PRESS VOL MINUS","SELECT AND VOL PLUS","SELECT AND VOL MINUS","LONG HOLD TO LOCK","LONG HOLD TO UNLOCK","POWER SHORT ONLY","FREE INPUT"};
const char *raw_prompt(uint32_t p){return names[p<20?p:20];}
static char *cp(char *p,const char *s){while(*s)*p++=*s++;return p;}
static char *hx(char *p,uint32_t n){static const char hex[]="0123456789ABCDEF";for(unsigned i=0;i<8;i++)*p++=hex[(n>>(28-4*i))&15];return p;}
static char *field(char *p,uint32_t value,int known){*p++=',';return known?hx(p,value):cp(p,"NA");}
static void row(struct raw_state *r,const struct input_services *io,const char *kind,uint32_t call_units,int pending)
{
 struct input_state *s=&r->session;if(s->log_status)return;char b[224];char *p=cp(b,kind);
 p=field(p,s->polls,1);p=field(p,s->elapsed,1);*p++=',';p=cp(p,r->armed?raw_prompt(s->prompt):"ARM");
 p=field(p,s->keys,!pending);p=field(p,s->pressed,!pending);p=field(p,s->released,!pending);
 p=field(p,r->before.state,1);p=field(p,r->before.event,1);
 p=field(p,r->after.state,!pending);p=field(p,r->after.event,!pending);p=field(p,call_units,1);*p++='\n';*p=0;
 input_journal_append(s,io,raw_log_path,b);
}
static int col(const struct input_services *io,unsigned r,unsigned g,unsigned b)
{const struct mmi_rgb888 c={(uint8_t)r,(uint8_t)g,(uint8_t)b};return !io->graphics.set_color(&c);}
static int rect(const struct input_services *io,int x,int y,unsigned w,unsigned h)
{const struct mmi_rect_s16 r={(int16_t)x,(int16_t)y,(uint16_t)w,(uint16_t)h};return !io->graphics.fill(&r);}
static int text(const struct input_services *io,int y,const char *str)
{
 if(!col(io,0,0,0)||!rect(io,4,y,312,16)||!col(io,232,240,248))return 0;
 int x=4;
 for(;*str;str++,x+=12){
  const unsigned char *g=*str>='A'&&*str<='Z'?input_letters[(unsigned)(*str-'A')]:*str>='0'&&*str<='9'?input_digits[(unsigned)(*str-'0')]:0;
  if(!g)continue;
  for(unsigned yy=0;yy<7;yy++){
   unsigned xx=0;
   while(xx<5){
    if(!(g[yy]&(16u>>xx))){xx++;continue;}
    unsigned start=xx;while(xx<5&&(g[yy]&(16u>>xx)))xx++;
    if(!rect(io,x+(int)start*2,y+(int)yy*2,(xx-start)*2,2))return 0;
   }
  }
 }
 return 1;
}
static int num(const struct input_services *io,int y,const char *label,uint32_t n)
{char b[32];char *p=cp(b,label);p=hx(p,n);*p=0;return text(io,y,b);}
static int display(struct raw_state *r,const struct input_services *io,int initial,int finished)
{
 struct input_state *s=&r->session;
 if(initial&&(!col(io,0,0,0)||!rect(io,0,0,320,240)))return 0;
 if(!text(io,4,"SPMP KEYS RAW")||!text(io,24,finished?"FINISHED":!r->armed?"RELEASE BUTTONS":labels[s->prompt<20?s->prompt:20])||
  !num(io,44,"KEY ",s->keys)||!num(io,64,"S0 ",r->before.state)||!num(io,84,"E0 ",r->before.event)||
  !num(io,104,"S1 ",r->after.state)||!num(io,124,"E1 ",r->after.event)||!num(io,144,"TIME ",s->elapsed))return 0;
 if(s->log_status>=1&&s->log_status<=4){
  static const char *const e[]={"","SD OPEN E ","SD SEEK E ","SD WRITE E ","SD CLOSE E "};
  if(!num(io,164,e[s->log_status],s->log_errno))return 0;
 }else if(!text(io,164,s->log_status?"SD LOG LIMIT":s->log_records?"SD LOG OK":"SD LOG CHECK"))return 0;
 if(!text(io,184,s->prompt==19?"CHOOSE CONTINUE":s->prompt==17?"LOCK WITH LONG HOLD":s->prompt==18?"UNLOCK WITH LONG HOLD":"RELEASE AFTER PRESS")||
    !num(io,204,"POLL ",s->polls)||!num(io,224,"ROWS ",s->log_records)||io->graphics.paint())return 0;
 s->frames++;return 1;
}
struct history {struct raw_snapshot before,after,pre;uint32_t key,have,have_pre;};
static int different(struct raw_snapshot a,struct raw_snapshot b){return a.state!=b.state||a.event!=b.event;}
static int poll_raw(struct raw_state *r,const struct input_services *io,raw_snapshot_fn read,struct history *h,int force)
{
 struct input_state *s=&r->session;read(&r->before);s->elapsed=io->time()-s->start;
 /* Persist changes BEFORE GetKeys: POWER may hand control to the OEM menu. */
 if(!h->have_pre||different(r->before,h->pre)){
  row(r,io,"PRE",0,1);h->pre=r->before;h->have_pre=1;
 }
 /* A second read narrows the gap introduced by the preceding file write. */
 read(&r->before);uint32_t t0=io->time();struct mmi_key_output32 key={0,0};
 uint32_t status=io->graphics.keys(&key);uint32_t t1=io->time();read(&r->after);
 s->elapsed=t1-s->start;r->samples++;
 if(status!=1)return 0;
 uint32_t old=s->keys;s->keys=key.keys;s->pressed=s->keys&~old;s->released=old&~s->keys;s->seen|=s->keys;
 if(s->keys)s->last=s->keys;
 uint32_t call=t1-t0;int changed=!h->have||different(r->before,h->before)||different(r->after,h->after)||s->keys!=h->key;
 if(changed){s->events++;row(r,io,"SAMPLE",call,0);}
 else if(force)row(r,io,"TICK",call,0);
 if(call>=1000)row(r,io,"GAP",call,0);
 h->before=r->before;h->after=r->after;h->key=s->keys;h->have=1;return 1;
}
static uint32_t phase(uint32_t elapsed)
{
 uint32_t end=0;
 for(uint32_t p=0;p<20;p++){
  end+=p<15?3000u:p<17?4000u:p<19?6000u:5000u;
  if(elapsed<end)return p;
 }
 return 20;
}
uint32_t raw_input_run(struct raw_state *r,const struct input_services *io,raw_snapshot_fn read,const struct raw_config *c)
{
 if(!r||!io||!read||!c||!c->duration_units||c->duration_units>80000||!c->max_polls||c->max_polls>4000||!c->redraw_units||
    !io->time||!io->delay||!io->open||!io->seek||!io->write||!io->close||!io->error_number)return 2;
 struct input_state *s=&r->session;struct probe_oem_adapter adapter={&io->graphics};
 if(probe_oem_operations()->begin(&adapter))return 2;
 uint16_t w=0,h=0;if(io->graphics.dimensions(&w,&h)||w!=320||h!=240||io->graphics.get_color(&s->original_color))return 2;
 s->color_saved=1;s->result=0;
 if(!display(r,io,1,0)){s->result=2;goto done;}
 input_journal_append(s,io,raw_log_path,"\n#SPMP_RAW_INPUT_V2,app=RAW_R1,path=/fat20a0/hda0/SPMPRAW.CSV,source=5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e,state=00556144,event=00556148,labels=PROMPTS_NOT_AUTODETECTED\n");
 input_journal_append(s,io,raw_log_path,"event,poll_hex,time_hex,prompt,cooked_hex,pressed_hex,released_hex,state_before_hex,event_before_hex,state_after_hex,event_after_hex,call_units_hex\n");
 s->start=io->time();struct history history;
 volatile uint8_t *cleared=(volatile uint8_t *)&history;
 for(unsigned i=0;i<sizeof(history);i++)cleared[i]=0;
 unsigned arming=0;
 for(;arming<150;arming++){
  if(!poll_raw(r,io,read,&history,arming==0)){s->result=2;goto done;}
  if(!(r->before.state|r->before.event|r->after.state|r->after.event|s->keys)){r->armed=1;break;}
  if(s->elapsed>=3000){s->result=2;goto done;}io->delay(2,0);
 }
 if(!r->armed){s->result=2;goto done;}
 uint32_t guide_start=s->elapsed,last_draw=0,last_tick=0,previous_events=s->events;
 row(r,io,"PROMPT",0,0);
 if(!display(r,io,0,0)){s->result=2;goto done;}
 while(s->polls<c->max_polls){
  s->elapsed=io->time()-s->start;if(s->elapsed>=c->duration_units)break;
  uint32_t next=phase(s->elapsed-guide_start);int prompt_changed=next!=s->prompt;
  if(prompt_changed){s->prompt=next;s->pressed=0;s->released=0;row(r,io,"PROMPT",0,0);}
  int tick=s->elapsed-last_tick>=1000;
  if(!poll_raw(r,io,read,&history,tick||prompt_changed)){s->result=2;break;}
  if(tick)last_tick=s->elapsed;
  if(prompt_changed||((s->events!=previous_events||tick)&&s->elapsed-last_draw>=c->redraw_units)){
   if(!display(r,io,0,0)){s->result=2;break;}last_draw=s->elapsed;previous_events=s->events;
  }
  s->polls++;io->delay(2,0);
 }
done:
 s->pressed=0;s->released=0;row(r,io,s->result?"END_ERROR":"END",0,0);
 if(!s->result){if(!display(r,io,0,1))s->result=2;else io->delay(25,0);}
 if(s->color_saved&&io->graphics.set_color(&s->original_color))s->result=2;
 return s->result;
}
