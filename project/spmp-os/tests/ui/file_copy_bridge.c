/* Test-only entry points, shared by host and ARM926 runs. No device package. */
#include "spmp_fm02_copy.h"
static struct spmp_fm02_copy context;
static fm01_model browser;
static fm02_model actions;
static char selected[FM01_NAME_MAX];
static uint32_t polls,cancel_at;
uint32_t copy_test_cookie=0x53504350; /* Retain a real RW data section in test ELF. */
uint32_t spmp_app_main(uintptr_t ignored){(void)ignored;return 0;}
int copy_test_init(const struct spmp_copy_io *io){context.sequence=0;return spmp_file_copy_init(&context.task,io);}
int copy_test_start(const char *s,const char *d,uint32_t token){return spmp_file_copy_start(&context.task,s,d,token);}
int copy_test_step(void){return spmp_file_copy_step(&context.task);}
void copy_test_cancel(void){spmp_file_copy_cancel(&context.task);}
int copy_test_cleanup(void){return spmp_file_copy_cleanup(&context.task);}
int copy_test_value(unsigned field){
 const struct spmp_file_copy *t=&context.task;
 switch(field){
 case 0:return (int)t->phase;case 1:return t->error;case 2:return t->cleanup_error;
 case 3:return (int)t->written;case 4:return (int)t->verified;case 5:return t->owned_stage;
 case 6:return t->published;case 7:return (int)t->attempts;case 8:return (int)t->original.bytes;
 case 9:return (int)sizeof *t;case 10:return (int)actions.state;
 case 11:return actions.progress.phase;
 default:return -1;
 }
}
static void text(char *d,const char *s){while((*d++=*s++)){} }
static int list(void *u,const char *p,uint32_t offset,uint32_t cap,fm01_entry *out,uint32_t *got,uint32_t *total,uint32_t *files,uint32_t *dirs){
 (void)u;(void)p;*got=cap&&!offset?1:0;*total=*files=1;*dirs=0;
 if(*got){*out=(fm01_entry){0};text(out->name,selected);out->type=FM01_ENTRY_FILE;}
 return 0;
}
static int cancel(void *u){(void)u;polls++;return cancel_at&&polls>=cancel_at;}
int copy_test_fm02(const char *src,const char *dst,const char *name,uint32_t stop){
 if(!fm02_valid_name(name))return -22;
 unsigned n=0;while(src[n]&&n<FM01_PATH_MAX)n++;if(n==FM01_PATH_MAX)return -22;
 n=0;while(dst[n]&&n<FM01_PATH_MAX)n++;if(n==FM01_PATH_MAX)return -22;
 text(selected,name);polls=0;cancel_at=stop;
 fm01_init(&browser,list,0);text(browser.panels[0].path,src);text(browser.panels[1].path,dst);
 fm02_init(&actions,&browser,spmp_fm02_copy_action,&context);fm02_set_cancel_poll(&actions,cancel,0);
 int r=fm02_prepare(&actions,FM02_COPY,0);return r?r:fm02_commit(&actions,1);
}
int copy_test_unsupported(void){fm02_request r={0};fm02_progress p;r.action=FM02_DELETE;return spmp_fm02_copy_action(&context,&r,0,0,&p);}
