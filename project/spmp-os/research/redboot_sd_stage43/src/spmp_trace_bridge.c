#include "spmp_trace_bridge.h"

typedef struct {
    const spmp_db_ops *o;
    spmp_bootlog *j;
    spmp_trace_tick tick;
} trace_context;
static uint32_t ticks(trace_context *t) {
    return t->tick?t->tick(t->o->context):0;
}
static void record(trace_context*t,uint16_t phase,uint16_t lvl,uint32_t code,
                   uint32_t a,uint32_t b,const char*msg) {
    spmp_log_event(t->j,ticks(t),phase,lvl,code,a,b,msg);
}
static int force_factory(void*c) {
    trace_context*t=(trace_context*)c;
    int r=t->o->force_factory?t->o->force_factory(t->o->context):0;
    record(t,SPMP_LOG_RED_BOOT,SPMP_LOG_INFO,0x430001u,(uint32_t)r,0,"forced factory key");
    return r;
}
static int read_package(void*c,const char*path,uint8_t*buf,size_t cap,size_t*got) {
    trace_context*t=(trace_context*)c;
    int r=t->o->read_package?t->o->read_package(t->o->context,path,buf,cap,got):0;
    record(t,SPMP_LOG_SD_SCAN,r==1?SPMP_LOG_INFO:SPMP_LOG_WARN,
           0x430002u,(uint32_t)r,got?(uint32_t)*got:0,"SD FAT BOOT.PKG read");
    return r;
}
static int restore(void*c) {
    trace_context*t=(trace_context*)c;
    int r=t->o->restore_factory_environment?t->o->restore_factory_environment(t->o->context):0;
    record(t,SPMP_LOG_RED_BOOT,r==1?SPMP_LOG_INFO:SPMP_LOG_ERROR,0x430003u,(uint32_t)r,0,"restore OEM clocks/pins");
    return r;
}
static int preflight(void*c,const uint8_t*e,size_t n,uint32_t*entry) {
    trace_context*t=(trace_context*)c;
    int r=t->o->preflight_elf?t->o->preflight_elf(t->o->context,e,n,entry):0;
    record(t,SPMP_LOG_SD_SCAN,r==1?SPMP_LOG_INFO:SPMP_LOG_ERROR,
           0x430004u,(uint32_t)r,entry?*entry:0,"RAM ownership preflight");
    return r;
}
static int stage(void*c,const uint8_t*e,size_t n,uint32_t*entry) {
    trace_context*t=(trace_context*)c;
    int r=t->o->stage_elf?t->o->stage_elf(t->o->context,e,n,entry):0;
    record(t,SPMP_LOG_SD_SCAN,r==1?SPMP_LOG_INFO:SPMP_LOG_ERROR,
           0x430005u,(uint32_t)r,entry?*entry:0,"copy/cache stage");
    return r;
}
static void jump_sd(void*c,uint32_t entry) {
    trace_context*t=(trace_context*)c;
    record(t,SPMP_LOG_RED_BOOT,SPMP_LOG_INFO,0x430006u,entry,0,"jump SPMP-OS");
    if(t->o->jump_sd)t->o->jump_sd(t->o->context,entry);
}
static void boot_factory(void*c) {
    trace_context*t=(trace_context*)c;
    record(t,SPMP_LOG_RED_BOOT,SPMP_LOG_WARN,0x430007u,0,0,"factory ROMFS handoff");
    if(t->o->boot_factory)t->o->boot_factory(t->o->context);
}
static void event(void*c,enum spmp_db_event ev) {
    trace_context*t=(trace_context*)c;
    record(t,SPMP_LOG_RED_BOOT,ev==SPMP_DB_EV_FACTORY_RESTORE_FAILED?SPMP_LOG_ERROR:SPMP_LOG_INFO,
           0x430100u+(uint32_t)ev,0,0,"dualboot policy");
    if(t->o->on_event)t->o->on_event(t->o->context,ev);
}
int spmp_trace_boot(const spmp_db_ops *platform,spmp_bootlog*journal,
                    uint8_t*staging,size_t capacity,spmp_trace_tick tick) {
    trace_context tc;
    spmp_db_ops wrapped;
    if(!platform || !platform->boot_factory)return -1;
    tc.o=platform;tc.j=journal;tc.tick=tick;
    wrapped.context=&tc;
    wrapped.verified_board_id=platform->verified_board_id;
    wrapped.force_factory=platform->force_factory?force_factory:0;
    wrapped.read_package=platform->read_package?read_package:0;
    wrapped.restore_factory_environment=platform->restore_factory_environment?restore:0;
    wrapped.preflight_elf=platform->preflight_elf?preflight:0;
    wrapped.stage_elf=platform->stage_elf?stage:0;
    wrapped.jump_sd=platform->jump_sd?jump_sd:0;
    wrapped.boot_factory=boot_factory;
    wrapped.on_event=event;
    record(&tc,SPMP_LOG_RED_BOOT,SPMP_LOG_INFO,0x430000u,
           (uint32_t)capacity,platform->verified_board_id,"Stage43 boot begin");
    return spmp_db_boot(&wrapped,staging,capacity);
}
