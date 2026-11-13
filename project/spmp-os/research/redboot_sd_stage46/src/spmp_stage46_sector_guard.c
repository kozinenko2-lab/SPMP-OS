#include "spmp_stage46_sector_guard.h"

static void zero_bytes(uint8_t *s, uint32_t n){
    volatile uint8_t *p=(volatile uint8_t *)s;
    while(n--) *p++=0;
}
static enum spmp46_code poll_code(enum spmp45_result c){
    switch(c){
    case SPMP45_LIMIT: return SPMP46_POLL_TIMEOUT;
    case SPMP45_STATUS_ERROR: return SPMP46_STATUS_ERROR;
    case SPMP45_READ_ERROR: return SPMP46_STATUS_IO_ERROR;
    case SPMP45_CANCELLED: return SPMP46_CANCELLED;
    default: return SPMP46_INVALID;
    }
}
spmp46_result spmp46_read_sector(const spmp46_reader *r,
                                 uint32_t lba, uint8_t *dest,
                                 uint32_t cap){
    spmp46_result out={SPMP46_INVALID,0,0,0,0};
    uint8_t scratch[SPMP46_SECTOR_BYTES];
    uint32_t i;
    /* Nothing calls callbacks before validation; prevents malformed config
       from touching devices or the caller's destination. */
    if(!r || !dest || cap!=SPMP46_SECTOR_BYTES || !r->begin_read ||
       !r->read_status || !r->finish_read || !r->recover ||
       !r->completion_mask || !r->poll_limit ||
       r->poll_limit>SPMP45_MAX_SAMPLES ||
       !r->attempts || r->attempts>SPMP46_MAX_ATTEMPTS)
        return out;
    for(i=0;i<r->attempts;i++){
        spmp45_poll_io p;
        spmp45_poll_result pr;
        uint32_t actual=0, flags=0;
        uint32_t j;
        out.attempts_used++;
        zero_bytes(scratch,SPMP46_SECTOR_BYTES);
        if(r->cancelled && r->cancelled(r->context)){
            out.code=SPMP46_CANCELLED;
            break;
        }
        if(r->begin_read(r->context,lba)!=1){
            out.code=SPMP46_PREPARE_ERROR;
            /* No transaction started, so controller recovery is not assumed
             * safe; backend must clean up its own failed begin_read. */
            break;
        }
        p.context=r->context;
        p.read_status=r->read_status;
        p.cancelled=r->cancelled;
        p.report=0;
        p.wanted_mask=r->completion_mask;
        p.max_samples=r->poll_limit;
        pr=spmp45_poll_status(&p);
        out.polls_total+=pr.polls;
        out.last_status=pr.last_status;
        if(pr.result==SPMP45_REACHED && pr.matched){
            if(r->finish_read(r->context,scratch,&actual,&flags)!=1 ||
               actual!=SPMP46_SECTOR_BYTES){
                out.code=SPMP46_DATA_ERROR;
            }else if((flags&SPMP46_REQUIRED_FLAGS)!=SPMP46_REQUIRED_FLAGS){
                out.code=(flags&SPMP46_FLAG_CRC_VALID) ?
                    SPMP46_DATA_ERROR : SPMP46_CRC_ERROR;
            }else{
                out.last_flags=flags;
                /* Commit only after the whole transaction has been checked. */
                for(j=0;j<SPMP46_SECTOR_BYTES;j++)dest[j]=scratch[j];
                out.code=SPMP46_OK;
                zero_bytes(scratch,SPMP46_SECTOR_BYTES);
                break;
            }
            out.last_flags=flags;
        }else{
            out.code=poll_code(pr.result);
        }
        zero_bytes(scratch,SPMP46_SECTOR_BYTES);
        if(r->recover(r->context)!=1){
            out.code=SPMP46_RECOVERY_ERROR;
            break;
        }
        /* A cancelled transaction is terminal even if recovery succeeded. */
        if(out.code==SPMP46_CANCELLED)break;
    }
    zero_bytes(scratch,SPMP46_SECTOR_BYTES);
    if(r->report)r->report(r->context,out.code,lba,out.attempts_used,
                          out.polls_total,out.last_status);
    return out;
}
