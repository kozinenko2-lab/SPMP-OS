#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "input_test.h"
static struct {uint32_t now,keys[64],key_index,open_calls,close_calls,writes,paints,step,errno_now,errno_calls;int fault;size_t size,pos;char file[1100000];struct mmi_rgb888 color;} m;
static uint32_t ok(void){return 0;}
static uint32_t origin(const struct mmi_display_origin16 *p){assert(!p->x&&!p->y);return 0;}
static uint32_t dbl(int32_t p){assert(p==1);return 0;}
static uint32_t dims(uint16_t *w,uint16_t *h){*w=320;*h=240;return 0;}
static uint32_t get(struct mmi_rgb888 *p){*p=m.color;return 0;}
static uint32_t set(const struct mmi_rgb888 *p){m.color=*p;return 0;}
static uint32_t fill(const struct mmi_rect_s16 *p){assert(p->x>=0&&p->y>=0&&p->x+p->width<=320&&p->y+p->height<=240);return 0;}
static uint32_t paint(void){m.paints++;return 0;}
static uint32_t keys(struct mmi_key_output32 *p){uint32_t i=m.key_index++;p->keys=m.fault==9?(i&1u):(i<64?m.keys[i]:0);return 1;}
static uint32_t time_now(void){return m.now;}
static void delay(uint32_t lo,uint32_t hi){assert(!hi);if(m.fault!=9)m.now+=m.step&&lo==2?m.step:lo*10;}
static int32_t open_file(const char *p,uint32_t flags,uint32_t mode){assert(!strcmp(p,"/fat20a0/hda0/SPMPKEYS.CSV")&&flags==0xa&&mode==0600);m.open_calls++;if(m.fault==1){m.errno_now=2;return -1;}return 0;}
static int32_t seek_file(int32_t fd,int32_t off,uint32_t whence){assert(fd==0&&off==0&&whence==2);if(m.fault==2||m.fault==10){m.errno_now=5;return -1;}m.pos=m.size;return (int32_t)m.pos;}
static int32_t write_file(int32_t fd,const void *p,uint32_t n){assert(fd==0);m.writes++;if(m.fault==3){m.errno_now=28;return -1;}if(m.fault==4)return 0;if(m.fault==5)return (int32_t)n+1;if(m.fault==6&&n>7)n=7;assert(m.pos+n<sizeof(m.file));memcpy(m.file+m.pos,p,n);m.pos+=n;if(m.pos>m.size)m.size=m.pos;return (int32_t)n;}
static int32_t close_file(int32_t fd){assert(fd==0);m.close_calls++;if(m.fault==7||m.fault==10){m.errno_now=9;return -1;}return 0;}
static uint32_t error_number(void){m.errno_calls++;return m.errno_now;}
static struct input_services services(void){struct input_services io={{ok,origin,dbl,dims,get,set,fill,paint,keys},time_now,delay,open_file,write_file,seek_file,close_file,error_number};return io;}
static struct input_state run(int fault,uint32_t initial_time)
{
 memset(&m,0,sizeof(m));m.fault=fault;m.now=initial_time;m.color=(struct mmi_rgb888){8,128,248};
 memcpy(m.file,"PREEXISTING\n",12);m.size=12;
 m.keys[1]=1;m.keys[3]=2;m.keys[5]=0x80000;
 if(fault==8)m.size=1048576;
 struct input_state s={0};struct input_services io=services();struct input_config c={160,12,40,20};
 if(fault==9)c=(struct input_config){60000,3000,4000,100};
 assert(input_test_run(&s,&io,&c)==0);
 assert(!memcmp(m.file,"PREEXISTING\n",12));
 assert(m.color.r==8&&m.color.g==128&&m.color.b==248);
 assert(m.close_calls==m.open_calls-(fault==1?1u:0u));
 return s;
}
static void menu_hold(void)
{
 memset(&m,0,sizeof(m));m.step=1000;
 for(unsigned i=0;i<13;i++)m.keys[1+4*i]=1u<<i;
 m.keys[53]=2;m.keys[54]=2;m.keys[55]=2; /* One DOWN edge held for three polls. */
 m.keys[57]=16; /* Select MAP using the learned A after releasing DOWN. */
 struct input_state s={0};struct input_services io=services();struct input_config c={60000,3000,4000,100};
 assert(input_test_run(&s,&io,&c)==0);
 assert(s.menu_choice==1&&s.menu_view==1&&s.sample_valid==0x1fff);
 assert(m.open_calls==m.close_calls&&s.log_status==0);
}
int main(void)
{
 struct input_state s=run(0,0);assert(s.log_status==0&&s.events==6&&s.seen==0x80003&&s.frames>1);
 m.file[m.size]=0;assert(strstr(m.file,"KEY,")&&strstr(m.file,"SAMPLE,")&&strstr(m.file,"END,")&&strstr(m.file,"00080000"));
 s=run(0,0xfffffff0u);assert(s.elapsed==160&&s.log_status==0);
 for(int fault=1;fault<=5;fault++){s=run(fault,0);assert(s.log_status==(fault==1?1u:fault==2?2u:3u));assert(s.polls==8);if(fault==2)assert(!m.writes);}
 s=run(6,0);assert(s.log_status==3); /* Eight short writes exhausted; bounded. */
 s=run(7,0);assert(s.log_status==4&&s.log_errno==9);
 s=run(1,0);assert(s.log_errno==2&&m.errno_calls==1);
 s=run(3,0);assert(s.log_errno==28&&m.errno_calls==1);
 s=run(4,0);assert(s.log_errno==0&&!m.errno_calls);
 s=run(10,0);assert(s.log_status==2&&s.log_errno==5&&m.errno_now==9&&m.errno_calls==1);
 s=run(8,0);assert(s.log_status==5&&!m.writes);
 s=run(9,0);assert(s.polls==3000&&s.log_status==5&&s.log_bytes<=65536);
 menu_hold();
 puts("PASS: input host, fd0, append preservation, transitions, timer wrap, SD/seek/write/close errors and byte/poll caps");
 return 0;
}
