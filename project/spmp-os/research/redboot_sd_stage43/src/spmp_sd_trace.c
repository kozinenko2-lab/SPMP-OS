#include "spmp_sd_trace.h"
static int traced_sector(void*context,uint32_t lba,uint8_t out[512]) {
    spmp_sd_trace*t=(spmp_sd_trace*)context;
    int rc=0;
    uint32_t before=0,after=0;
    if(!t || !out)return 0;
    if(t->clock)before=t->clock(t->base.context);
    if(t->base.read_sector)rc=t->base.read_sector(t->base.context,lba,out);
    if(t->clock)after=t->clock(t->base.context);
    t->reads++;
    if(rc!=1)t->failures++;
    if(t->reads_since_sample!=0xffffffffu)t->reads_since_sample++;
    if(rc!=1 || (t->sampled_every &&
        (t->reads_since_sample>=t->sampled_every || t->reads<=8))) {
        t->reads_since_sample=0;
        spmp_log_event(t->journal,after,SPMP_LOG_SD_SCAN,
                       rc==1?SPMP_LOG_DEBUG:SPMP_LOG_ERROR,
                       rc==1?0x430201u:0x430202u,
                       lba,after-before,rc==1?"read-sector OK":"read-sector failed");
    }
    return rc;
}
spmp_fat_io spmp_sd_trace_io(spmp_sd_trace*t) {
    spmp_fat_io out={0};
    if(!t)return out;
    out.context=t;
    out.physical_sectors=t->base.physical_sectors;
    out.max_reads=t->base.max_reads;
    out.read_sector=traced_sector;
    return out;
}
