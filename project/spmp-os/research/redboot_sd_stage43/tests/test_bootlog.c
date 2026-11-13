#define _GNU_SOURCE
#include "spmp_bootlog.h"
#include "spmp_trace_bridge.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {char buf[1024*128];size_t len;int fail_after;int synced;} memory_sink;
static int append(void*ctx,const char*b,size_t n) {
 memory_sink*s=ctx;
 if((s->fail_after>=0 && (int)s->len>=s->fail_after)||s->len+n>sizeof(s->buf))return -1;
 memcpy(s->buf+s->len,b,n);s->len+=n;return (int)n;
}
static int sync_fn(void*ctx){((memory_sink*)ctx)->synced++;return 1;}
static unsigned count_lines(const char*buf,size_t n){unsigned c=0;for(size_t i=0;i<n;i++)c+=(buf[i]=='\n');return c;}
struct fsim {int boot_factory;int restore;int invalid;int force;int stage;int jump;int event_count;};
static int fforce(void*c){return ((struct fsim*)c)->force;}
static int freadpkg(void*c,const char*path,uint8_t*data,size_t cap,size_t*n){
 struct fsim*f=c;assert(strcmp(path,"SPMPOS/BOOT.PKG")==0);
 if(cap<110)return 0;
 memset(data,0,cap);memcpy(data,"SPMPDB01",8);
 data[8]=1;data[12]=48;data[16]=52;data[24]=1;data[28]=1;
 memcpy(data+48,"\x7f""ELF\x01\x01\x01",7);data[48+18]=40;
 uint32_t crc=spmp_db_crc32(data+48,52);
 data[20]=(uint8_t)crc;data[21]=(uint8_t)(crc>>8);data[22]=(uint8_t)(crc>>16);data[23]=(uint8_t)(crc>>24);
 if(f->invalid)data[20]^=1;
 *n=100;return 1;
}
static int frestore(void*c){((struct fsim*)c)->restore++;return 1;}
static int fpreflight(void*c,const uint8_t*e,size_t n,uint32_t*entry){
 (void)c;(void)e;(void)n;*entry=0x400000;return 1;
}
static int fstage(void*c,const uint8_t*e,size_t n,uint32_t*entry){
 (void)e;(void)n;((struct fsim*)c)->stage++;*entry=0x400000;return 1;
}
static void fjump(void*c,uint32_t entry){assert(entry==0x400000);((struct fsim*)c)->jump++;}
static void ffactory(void*c){((struct fsim*)c)->boot_factory++;}
static void fevent(void*c,enum spmp_db_event e){(void)e;((struct fsim*)c)->event_count++;}
static uint32_t ftick(void*c){(void)c;return 123;}
static void test_wrap_and_recovery(void){
 static spmp_bootlog log;
 uint32_t written=0;
 memory_sink sink={.len=0,.fail_after=-1,.synced=0};
 spmp_log_sink out={&sink,append,sync_fn};
 spmp_log_init(&log,0x1234,0);
 assert(spmp_log_is_valid(&log));
 for(unsigned i=0;i<350;i++)spmp_log_event(&log,i,1,1,i,i+1,i+2,"hello");
 assert(log.count==256 && log.dropped==94);
 assert(spmp_log_is_valid(&log));
 spmp_log_console_char(&log,0,1,'A');spmp_log_console_char(&log,0,1,'B');spmp_log_console_char(&log,0,1,'\n');
 assert(log.count==256 && log.dropped==95);
 assert(spmp_log_export_text(&log,&out,1000,&written)==0);
 assert(written==256 && sink.synced==1);
 assert(count_lines(sink.buf,sink.len)==257);
 assert(memmem(sink.buf,sink.len,"dropped=95",10)!=NULL);
 sink.fail_after=0;assert(spmp_log_export_text(&log,&out,1,&written)==-2);
 sink.fail_after=-1;
 spmp_log_init(&log,0x5678,1);assert(log.count==256 && log.session==0x5678);
 log.entries[0].message[0]^=1;
 assert(!spmp_log_is_valid(&log));
 spmp_log_init(&log,0x9101,1);assert(log.count==0 && log.dropped==0);
 struct fsim f={0};
 spmp_db_ops p={0};
 p.context=&f;p.verified_board_id=1;p.force_factory=fforce;p.read_package=freadpkg;
 p.restore_factory_environment=frestore;p.preflight_elf=fpreflight;p.stage_elf=fstage;
 p.jump_sd=fjump;p.boot_factory=ffactory;p.on_event=fevent;
 uint8_t data[110];
 assert(spmp_trace_boot(&p,&log,data,sizeof(data),ftick)==-2);
 assert(f.jump==1 && f.boot_factory==1 && f.restore==1 && f.stage==1);
 assert(spmp_log_is_valid(&log) && log.count>=7);
 f.invalid=1;
 assert(spmp_trace_boot(&p,&log,data,sizeof(data),ftick)==-2);
 assert(f.jump==1 && f.boot_factory==2 && f.restore==2);
 f.force=1;
 assert(spmp_trace_boot(&p,&log,data,sizeof(data),ftick)==-2);
 assert(f.restore==2 && f.boot_factory==3);
 printf("OK: ring overwrite/recovery/SD success+fallback/forced boot/export\n");
}
int main(void){test_wrap_and_recovery();return 0;}
