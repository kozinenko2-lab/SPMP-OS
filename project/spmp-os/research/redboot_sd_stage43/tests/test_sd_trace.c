#include "spmp_sd_trace.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {uint32_t ticks;int fail;} dev;
static int read_sector(void*ctx,uint32_t lba,uint8_t out[512]) {
 dev*d=ctx;d->ticks+=3;
 if(d->fail==1)return 0;
 memset(out,(unsigned char)lba,512);
 return 1;
}
static uint32_t tick(void*ctx){return ((dev*)ctx)->ticks;}
int main(void){
 static spmp_bootlog log;
 dev device={0};
 spmp_log_init(&log,11,0);
 spmp_sd_trace sd={0};
 sd.base.context=&device;sd.base.physical_sectors=4096;
 sd.base.max_reads=100;sd.base.read_sector=read_sector;
 sd.journal=&log;sd.clock=tick;sd.sampled_every=1;
 spmp_fat_io io=spmp_sd_trace_io(&sd);
 uint8_t block[512];
 for(uint32_t i=0;i<12;i++)assert(io.read_sector(io.context,i,block)==1);
 assert(log.count==12 && sd.reads==12 && sd.failures==0);
 assert(log.entries[0].arg0==0 && log.entries[0].arg1==3);
 device.fail=1;
 assert(io.read_sector(io.context,20,block)==0);
 assert(sd.failures==1 && log.count==13);
 sd.sampled_every=0;
 assert(io.read_sector(io.context,20,block)==0);
 assert(log.count==14);
 device.fail=0;
 assert(io.read_sector(io.context,22,block)==1);
 assert(log.count==14);
 puts("OK: read-only FAT sector trace, timing, failures, verbosity");
 return 0;
}
