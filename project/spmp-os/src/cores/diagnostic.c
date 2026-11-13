#include "spmp_cores.h"
/* Original SPMP-OS diagnostic test module: tests host callbacks and frame flow.
 * This is NOT an emulator and does not drive real SPMP8000 hardware.
 */
static const spmp_core_host *host_ref;
static uint16_t frame[320u*240u];
static unsigned phase;
static int active;
static int diag_start(const spmp_core_host *h,const char *content){
    (void)content;
    if(!h||!h->present||active)return -1;
    host_ref=h;phase=0;active=1;
    return 0;
}
static int diag_tick(void){
    if(!active||!host_ref)return -1;
    uint32_t buttons=host_ref->buttons?host_ref->buttons(host_ref->user):0;
    for(unsigned y=0;y<240;y++)for(unsigned x=0;x<320;x++){
        unsigned r=((x+phase*9u)>>3)&31u;
        unsigned g=((y+phase*5u)>>2)&63u;
        unsigned b=(((x^y)+(buttons&31u))>>3)&31u;
        frame[y*320u+x]=(uint16_t)((r<<11)|(g<<5)|b);
    }
    /* Moving white marker confirms actual frame updates. */
    unsigned left=(phase*13u)%304u;
    for(unsigned y=112;y<128;y++)for(unsigned x=left;x<left+16;x++)
        frame[y*320u+x]=0xffffu;
    host_ref->present(host_ref->user,frame,320,240);
    phase++;
    return 0;
}
static void diag_stop(void){host_ref=0;active=0;phase=0;}
const spmp_core_ops spmp_diagnostic_ops={diag_start,diag_tick,diag_stop,NULL,NULL,NULL,NULL};
