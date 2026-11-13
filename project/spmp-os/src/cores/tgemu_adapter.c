/* SPMP-OS integrated PC Engine HuCard core, adapted from TGEmu.
 * Original TGEmu GPL-2.0-or-later source in third_party/tgemu/src.
 * No BIOS, game assets, eCos or RedBoot entry points are used here.
 */
#include "spmp_cores.h"
#include "shared.h"
#include <string.h>
#define SPMP_PCE_ROM_CAP 0x100000u
#define SPMP_PCE_FB_W 576u
#define SPMP_PCE_FB_H 256u
#define SPMP_PCE_OUT_W 320u
#define SPMP_PCE_OUT_H 240u
static uint16_t source_fb[SPMP_PCE_FB_W*SPMP_PCE_FB_H];
static uint16_t output_fb[SPMP_PCE_OUT_W*SPMP_PCE_OUT_H];
static int16_t audio_buf[2u*800u];
static const spmp_core_host *pce_host;
extern void spmp_tgemu_save_bind(const spmp_core_host *,const char *);
static int active;
static int pce_start(const spmp_core_host *host,const char *path) {
    if(!host || !host->present || !host->read_entire || !path || active) return -1;
    size_t got=0;
    /* Single in-place HuCard buffer: no duplicate 1 MiB input staging area.
           SPMP-OS VFS refuses files larger than the destination capacity. */
        if(host->read_entire(host->user,path,rom,sizeof rom,&got)) return -1;
    if(got<0x2000) return -1; /* Cartridge pages must contain at least 8KiB */
    size_t skip=(got%0x2000u==512u)?512u:0u;
    size_t bytes=got-skip;
    if(bytes>SPMP_PCE_ROM_CAP || (bytes%0x2000u))return -1;
    if(skip)memmove(rom,rom+skip,bytes);
    if(bytes<sizeof rom)memset(rom+bytes,0xff,sizeof rom-bytes);
    memset(source_fb,0,sizeof source_fb);
    memset(&bitmap,0,sizeof bitmap);
    bitmap.data=(uint8 *)source_fb;
    bitmap.width=(int)SPMP_PCE_FB_W;
    bitmap.height=(int)SPMP_PCE_FB_H;
    bitmap.depth=16;
    bitmap.granularity=2;
    bitmap.pitch=(int)(SPMP_PCE_FB_W*2u);
    bitmap.viewport.x=32;
    bitmap.viewport.y=0;
    bitmap.viewport.w=256;
    bitmap.viewport.h=240;
    input.pad[0]=0;
    input.system=0;
    pce_host=host;
    spmp_tgemu_save_bind(host,path);
    if(!system_init(host->audio_pcm ? 22050 : 0)) {spmp_tgemu_save_bind(0,0);pce_host=0;return -1;}
    system_reset();
    active=1;
    return 0;
}
static int pce_tick(void) {
    if(!active||!pce_host)return -1;
    uint32_t keys=pce_host->buttons?pce_host->buttons(pce_host->user):0u;
    /* SPMP-OS gamepad bits (not the old GE/eCos scan codes):
       0 up, 1 down, 2 left, 3 right, 4 B1, 5 B2, 6 start, 7 select. */
    input.pad[0]=0;
    if(keys&(1u<<0))input.pad[0]|=INPUT_UP;
    if(keys&(1u<<1))input.pad[0]|=INPUT_DOWN;
    if(keys&(1u<<2))input.pad[0]|=INPUT_LEFT;
    if(keys&(1u<<3))input.pad[0]|=INPUT_RIGHT;
    if(keys&(1u<<4))input.pad[0]|=INPUT_B1;
    if(keys&(1u<<5))input.pad[0]|=INPUT_B2;
    if(keys&(1u<<6))input.pad[0]|=INPUT_RUN;
    if(keys&(1u<<7))input.pad[0]|=INPUT_SELECT;
    system_frame(0);
    int vw=bitmap.viewport.w;
    int vh=bitmap.viewport.h;
    int vx=bitmap.viewport.x;
    int vy=bitmap.viewport.y;
    if(vw<1||vw>512||vh<1||vh>256||vx<0||vx+vw>bitmap.width||
        vy<0||vy+vh>bitmap.height)return -1;
    for(unsigned y=0;y<SPMP_PCE_OUT_H;y++){
        unsigned sy=(unsigned)vy+((y*(unsigned)vh)/SPMP_PCE_OUT_H);
        for(unsigned x=0;x<SPMP_PCE_OUT_W;x++){
            unsigned sx=(unsigned)vx+((x*(unsigned)vw)/SPMP_PCE_OUT_W);
            output_fb[y*SPMP_PCE_OUT_W+x]=source_fb[sy*SPMP_PCE_FB_W+sx];
        }
    }
    pce_host->present(pce_host->user,output_fb,SPMP_PCE_OUT_W,SPMP_PCE_OUT_H);
    if(pce_host->audio_pcm && snd.enabled && snd.buffer_size>0 && snd.buffer_size<=800){
        for(int i=0;i<snd.buffer_size;i++){
            audio_buf[i*2]=snd.buffer[0][i];
            audio_buf[i*2+1]=snd.buffer[1][i];
        }
        pce_host->audio_pcm(pce_host->user,audio_buf,(size_t)snd.buffer_size,22050u);
    }
    return 0;
}
static void pce_stop(void){
    if(active){system_shutdown();active=0;}
    pce_host=0;
    spmp_tgemu_save_bind(0,0);
}
const spmp_core_ops spmp_pcengine_ops={pce_start,pce_tick,pce_stop,NULL,NULL,NULL,NULL};
