/* Reuse RAM-only mock FS/graphics from the preceding input test. */
#define main legacy_input_test_main
#include "test_input_host.c"
#undef main
#include "raw_input.h"
static int32_t raw_open(const char *p,uint32_t flags,uint32_t mode)
{
 assert(!strcmp(p,"/fat20a0/hda0/SPMPRAW.CSV"));
 return open_file("/fat20a0/hda0/SPMPKEYS.CSV",flags,mode);
}
static void snapshot(struct raw_snapshot *out)
{
 out->state=m.key_index>=2&&m.key_index<5?0x40:0;
 out->event=m.key_index>=3&&m.key_index<5?0x400:0;
}
static struct raw_state raw_run(int fault,int full)
{
 memset(&m,0,sizeof(m));m.fault=fault;m.color=(struct mmi_rgb888){8,128,248};
 memcpy(m.file,"PREEXISTING\n",12);m.size=12;if(full)m.step=1000;
 struct input_services io=services();io.open=raw_open;
 struct raw_state s={0};struct raw_config c=full?(struct raw_config){80000,4000,100}:(struct raw_config){160,12,20};
 assert(raw_input_run(&s,&io,snapshot,&c)==0);
 assert(!memcmp(m.file,"PREEXISTING\n",12));assert(m.close_calls==m.open_calls-(fault==1?1u:0u));
 assert(m.color.r==8&&m.color.g==128&&m.color.b==248);
 return s;
}
int main(void)
{
 struct raw_state s=raw_run(0,0);m.file[m.size]=0;
 assert(s.armed&&s.session.log_status==0&&s.session.keys==0);
 assert(strstr(m.file,"#SPMP_RAW_INPUT_V2")&&strstr(m.file,"PRE,")&&strstr(m.file,"NA,NA,NA,")&&strstr(m.file,"00000040")&&strstr(m.file,"00000400")&&strstr(m.file,"END,"));
 assert(!strstr(m.file,"BACK")&&!strstr(m.file,"MENU"));
 s=raw_run(1,0);assert(s.session.log_status==1&&s.session.log_errno==2&&s.session.polls==8);
 s=raw_run(10,0);assert(s.session.log_status==2&&s.session.log_errno==5);
 s=raw_run(0,1);m.file[m.size]=0;
 assert(s.session.elapsed==80000&&s.session.prompt==20);
 for(unsigned p=0;p<=20;p++)assert(strstr(m.file,raw_prompt(p)));
 assert(strstr(m.file,"POWER_SHORT")&&strstr(m.file,"HOLD_LOCK")&&strstr(m.file,"HOLD_UNLOCK")&&strstr(m.file,"SELECT_VOL_PLUS")&&strstr(m.file,"SELECT_VOL_MINUS"));
 puts("PASS: raw host, hidden raw masks with cooked0, PRE unknown fields, SD/errno, all phases and distinct POWER/HOLD/volume chords");
 return 0;
}
