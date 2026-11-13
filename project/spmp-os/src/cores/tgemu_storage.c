/* SPMP-OS per-game save bridge; no eCos hardcoded paths or implicit writes.
 * Only host-provided callbacks perform persistent file operations.
 */
#include "spmp_cores.h"
#include "shared.h"
#include <string.h>
static const spmp_core_host *bound_host;
static const char *bound_content;
void spmp_tgemu_save_bind(const spmp_core_host *host,const char *content){
    bound_host=host;bound_content=content;
}
int load_file(char *ignored,uint8 *dst,int size){
    (void)ignored;
    if(!dst||size<0)return 0;
    memset(dst,0,(size_t)size);
    if(!bound_host||!bound_content||!bound_host->load_save)return 0;
    size_t got=0;
    int rc=bound_host->load_save(bound_host->user,"pcengine",bound_content,
                                  dst,(size_t)size,&got);
    if(rc||got!=(size_t)size){memset(dst,0,(size_t)size);return 0;}
    return 1;
}
int save_file(char *ignored,uint8 *src,int size){
    (void)ignored;
    if(!src||size<0||!bound_host||!bound_content||!bound_host->store_save)return 0;
    return bound_host->store_save(bound_host->user,"pcengine",bound_content,
                                  src,(size_t)size)==0;
}
