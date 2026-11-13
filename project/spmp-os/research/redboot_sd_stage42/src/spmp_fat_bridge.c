#include "spmp_fat_bridge.h"
#include "spmp_dualboot.h"
/* Integrates the FAT layer with Stage41's read_package callback.
 * Caller must set spmp_db_ops.context to a platform context embedding this
 * read-only driver, or route its original context through a wrapper.
 * No actual SD controller registers are accessed here. */
int spmp_fat_bridge_read_package(void *context,const char *path,
                                 uint8_t *output,size_t capacity,size_t *actual) {
    static const char expected[]=SPMP_DB_DEFAULT_PATH;
    size_t i;
    spmp_fat_bridge_context *c=(spmp_fat_bridge_context*)context;
    if(actual)*actual=0;
    if(!c||!path||!actual)return 0;
    for(i=0;i<sizeof(expected);i++)if(path[i]!=expected[i])return 0;
    return spmp_fat_read_boot(&c->io,output,capacity,actual);
}
