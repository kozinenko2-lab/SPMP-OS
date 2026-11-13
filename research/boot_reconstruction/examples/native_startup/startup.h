#ifndef SPMP_RESEARCH_STARTUP_H
#define SPMP_RESEARCH_STARTUP_H
#include <stdint.h>
/* Raw OEM table is opaque until a separate verified binder is implemented.
 * No launch information is read from r1-r3. No new stack or heap is allocated.
 */
enum spmp_startup_status { SPMP_STARTUP_UNBOUND=1, SPMP_STARTUP_NO_API=2 };
struct spmp_startup_record {
    uint32_t abi_version;
    uint32_t raw_api_address;
    uint32_t data_cookie;
    uint32_t status;
};
_Static_assert(sizeof(uintptr_t)==4,"ARM32 startup only");
_Static_assert(sizeof(struct spmp_startup_record)==16,"Startup record layout");
extern struct spmp_startup_record spmp_startup_record;
uint32_t spmp_app_main(uintptr_t raw_api_address);
#endif
