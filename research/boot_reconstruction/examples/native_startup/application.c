#include "startup.h"
struct spmp_startup_record spmp_startup_record;
/* A nonzero .data object exercises preservation independently of BSS clearing. */
static volatile uint32_t initialized_cookie=0x53504d50u;
uint32_t spmp_app_main(uintptr_t raw_api_address)
{
    spmp_startup_record.abi_version=1;
    spmp_startup_record.raw_api_address=(uint32_t)raw_api_address;
    spmp_startup_record.data_cookie=initialized_cookie;
    spmp_startup_record.status=raw_api_address?SPMP_STARTUP_UNBOUND:SPMP_STARTUP_NO_API;
    return spmp_startup_record.status;
}
