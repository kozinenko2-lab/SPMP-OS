#include "spmp_stage45_sd_poll.h"
spmp45_poll_result spmp45_poll_status(const spmp45_poll_io *io) {
    spmp45_poll_result out={SPMP45_INVALID,0,0,0,0};
    if(!io || !io->read_status || !io->wanted_mask ||
       !io->max_samples || io->max_samples>SPMP45_MAX_SAMPLES)return out;
    for(uint32_t i=0;i<io->max_samples;i++){
        uint32_t status=0;
        if(io->cancelled && io->cancelled(io->context)){
            out.result=SPMP45_CANCELLED;break;
        }
        if(io->read_status(io->context,&status)!=1){
            out.result=SPMP45_READ_ERROR;break;
        }
        out.polls++;
        out.last_status=status;
        out.error_status=status & SPMP45_OEM_ERROR_BITS;
        out.matched=(status & io->wanted_mask)!=0;
        if(out.error_status){out.result=SPMP45_STATUS_ERROR;break;}
        if(out.matched){out.result=SPMP45_REACHED;break;}
        if(i+1==io->max_samples)out.result=SPMP45_LIMIT;
    }
    if(io->report)io->report(io->context,out.result,out.polls,out.last_status,io->wanted_mask);
    return out;
}
