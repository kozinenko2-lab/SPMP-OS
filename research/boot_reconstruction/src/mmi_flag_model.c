/* Reconstruction of immediate/poll path 0x33CE74..0x33CEF4, not blocking wait.
 * Host only: no OEM object access, scheduler locks or target pointers.
 */
#include "mmi_flag_model.h"
uint32_t mmi_flag_poll_model(uint32_t *bits,uint32_t mask,unsigned mode)
{
    uint32_t result=0;
    if(mode>3) return 0; /* Added host input guard; OEM asserts on bad mode. */
    if(mode & MMI_FLAG_OR) {
        if(mask & *bits) result=*bits;
    } else {
        if(mask!=0 && (*bits & mask)==mask) result=*bits;
    }
    if(result!=0 && (mode & MMI_FLAG_CLEAR)) *bits=0;
    return result;
}
