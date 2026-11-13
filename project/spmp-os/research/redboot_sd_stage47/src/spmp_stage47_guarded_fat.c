#include "spmp_stage47_guarded_fat.h"

static uint32_t add_saturate(uint32_t v) {return v==UINT32_MAX?v:v+1u;}

int spmp47_command_argument(uint32_t lba, enum spmp47_address_mode mode,
                            uint32_t *argument) {
    if (!argument) return 0;
    *argument=0;
    if (mode==SPMP47_ADDR_BLOCKS) { *argument=lba; return 1; }
    if (mode==SPMP47_ADDR_BYTES && lba<=UINT32_MAX/512u) {
        *argument=lba*512u;return 1;
    }
    return 0;
}

int spmp47_guarded_fat_init(spmp47_guarded_fat *f,
                            const spmp46_reader *reader,
                            uint32_t physical_sectors, uint32_t max_reads,
                            spmp_bootlog *log, uint32_t verbose) {
    if(!f)return 0;
    /* Reset before validation: an invalid setup NEVER leaves a ready bridge. */
    f->bridge.io=(spmp_fat_io){0};
    f->trusted_reader=(spmp46_reader){0};
    f->optional_ram_log=0;
    f->successful_sectors=f->failed_sectors=0;
    f->last_lba=f->last_code=f->in_sector=f->verbose=0;
    if(!reader || !physical_sectors || !max_reads ||
       !reader->begin_read || !reader->read_status || !reader->finish_read ||
       !reader->recover || !reader->poll_limit ||
       reader->poll_limit>SPMP45_MAX_SAMPLES || !reader->completion_mask ||
       !reader->attempts || reader->attempts>SPMP46_MAX_ATTEMPTS)
       return 0;
    f->trusted_reader=*reader;
    f->optional_ram_log=log;
    f->verbose=(verbose!=0);
    f->bridge.io.context=f;
    f->bridge.io.physical_sectors=physical_sectors;
    f->bridge.io.max_reads=max_reads;
    f->bridge.io.read_sector=spmp47_guarded_sector;
    return 1;
}

int spmp47_guarded_sector(void *context, uint32_t lba, uint8_t out[512]) {
    spmp47_guarded_fat *f=(spmp47_guarded_fat *)context;
    spmp46_result r;
    if(!f || !out || !f->bridge.io.read_sector ||
       f->bridge.io.read_sector!=spmp47_guarded_sector ||
       f->in_sector || lba>=f->bridge.io.physical_sectors)
       return 0;
    f->in_sector=1;
    r=spmp46_read_sector(&f->trusted_reader,lba,out,512);
    f->in_sector=0;
    f->last_lba=lba; f->last_code=(uint32_t)r.code;
    if (r.code==SPMP46_OK) f->successful_sectors=add_saturate(f->successful_sectors);
    else f->failed_sectors=add_saturate(f->failed_sectors);
    if(f->optional_ram_log && (r.code!=SPMP46_OK || f->verbose)) {
       /* No filesystem IO, malloc, blocking UART or SD recursion here. */
       spmp_log_event(f->optional_ram_log,0,SPMP_LOG_SD_SCAN,
           r.code==SPMP46_OK ? SPMP_LOG_DEBUG:SPMP_LOG_ERROR,
           0x4700u+(uint32_t)r.code,lba,r.last_status,
           r.code==SPMP46_OK ? "SD sector verified" : "SD sector failed");
    }
    return r.code==SPMP46_OK;
}

int spmp47_read_package(void *context, const char *path,
                        uint8_t *output, size_t capacity, size_t *actual) {
    spmp47_guarded_fat *f=(spmp47_guarded_fat *)context;
    if(actual)*actual=0;
    if(!f || !f->bridge.io.read_sector ||
       f->bridge.io.read_sector!=spmp47_guarded_sector)return 0;
    return spmp_fat_bridge_read_package(&f->bridge,path,output,capacity,actual);
}
