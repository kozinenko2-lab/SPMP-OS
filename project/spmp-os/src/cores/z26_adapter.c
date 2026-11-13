/* SPMP-OS z26 6507/TIA/RIOT integration from user-supplied GPLv2 SPMP port.
 * Platform glue is independent of libgame/SDL/eCos; limited to joystick ROMs.
 */
#include "spmp_cores.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef unsigned int dd;
typedef unsigned short dw;
typedef unsigned char db;
typedef struct {uint8_t keys[128];} ge_key_data_t;
typedef struct {int dummy;} mcatch_loadimg_t;
FILE *zlog;
unsigned NativeGE_getTime(void){return 0;}
/* Modern C prototypes for functions originally declared implicitly. */
void ControlSWCHAWrite(void);
void srv_print(char*);
void QueueSoundBytes(void);
void TestLightgunHit(dd,dd);
void srv_SetPalette(void);
void ClearScreenBuffers(void);
void srv_Cleanup(void);
void cli_LoadNextStarpath(void);
void cli_ReloadStarpath(void);
void set_status(char *);
void Init_Service(void);
void Controls(void);
void srv_Events(void);
void srv_CopyScreen(void);
int srv_done;
static uint8_t *spmp_tia_row(unsigned row);
#include "globals.c"
#include "ct.c"
#include "carts.c"
#include "kidvid.c"
#include "palette.c"
#include "position.c"
#include "2600core.c"

/* z26 previously allocated 16,653 tiny 160-byte blocks on each reset.
 * Use one managed pool to avoid fragmentation and repeated-launch leaks. */
#define SPMP_TIA_ROWS 16653u
#define SPMP_TIA_ROW_SIZE 160u
static uint8_t *spmp_tia_pool;
static uint8_t *spmp_tia_row(unsigned row){
    if(!spmp_tia_pool || row>=SPMP_TIA_ROWS)return 0;
    return spmp_tia_pool + row*SPMP_TIA_ROW_SIZE;
}
static const spmp_core_host *spmp_2600_host;
static uint16_t spmp_2600_frame[320u*240u];
static int spmp_2600_active;
void ControlSWCHAWrite(void) {}
void srv_print(char *message) { (void)message; }
void TestLightgunHit(dd clock, dd line) { (void)clock;(void)line; }
void srv_SetPalette(void) { GeneratePalette(); }
void ClearScreenBuffers(void) { memset(RealScreenBuffer1,0,sizeof RealScreenBuffer1); }
void srv_Cleanup(void) {}
void cli_LoadNextStarpath(void) {}
void cli_ReloadStarpath(void) {}
void set_status(char *text) { (void)text; }
void srv_Events(void) {}
void Init_Service(void) { GeneratePalette(); }
void Controls(void) {
    uint32_t buttons=spmp_2600_host&&spmp_2600_host->buttons ? spmp_2600_host->buttons(spmp_2600_host->user) : 0;
    /* Right & left controller lines normally high, pulled low when pressed. */
    IOPortA_Controllers=0xff;
    if(buttons & (1u<<0)) IOPortA_Controllers &= ~(1u<<4); /* up */
    if(buttons & (1u<<1)) IOPortA_Controllers &= ~(1u<<5); /* down */
    if(buttons & (1u<<2)) IOPortA_Controllers &= ~(1u<<6); /* left */
    if(buttons & (1u<<3)) IOPortA_Controllers &= ~(1u<<7); /* right */
    InputLatch[0] = (buttons & (1u<<4))?0:0x80; /* fire */
}
void srv_CopyScreen(void) {}
static int z26_start(const spmp_core_host *host,const char *path){
    if(spmp_2600_active || !host || !host->read_entire || !host->present || !path)return -1;
    size_t rom_size=0;
    memset(CartRom,0xff,sizeof CartRom);
    if(host->read_entire(host->user,path,CartRom,128*1024,&rom_size))return -1;
    if(rom_size!=2048 && rom_size!=4096 && rom_size!=8192 && rom_size!=16384 && rom_size!=32768 && rom_size!=65536 && rom_size!=131072)return -1;
    CartSize=(int)rom_size;
    crc=0; /* No CRC game-specific overrides during preliminary integration */
    spmp_tia_pool=(uint8_t *)malloc(SPMP_TIA_ROWS*SPMP_TIA_ROW_SIZE);
    if(!spmp_tia_pool)return -1;
    spmp_2600_host=host;
    ScreenBuffer=RealScreenBuffer1;
    DisplayPointer=(dw*)RealScreenBuffer1;
    memset(RiotRam,0,sizeof RiotRam);
    def_LoadDefaults();
    Reset_emulator();
    spmp_2600_active=1;
    return 0;
}
static int z26_tick(void){
    if(!spmp_2600_active || !spmp_2600_host)return -1;
    Controls();
    ScanFrame();
    unsigned max_line=MaxLines>240?240:MaxLines;
    for(unsigned y=0;y<240;y++){
        for(unsigned x=0;x<320;x++){
            unsigned c=y<max_line?RealScreenBuffer1[y*320+x]&0x7f:0;
            unsigned r=PCXPalette[c*3],g=PCXPalette[c*3+1],b=PCXPalette[c*3+2];
            spmp_2600_frame[y*320+x]=(uint16_t)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
        }
    }
    spmp_2600_host->present(spmp_2600_host->user,spmp_2600_frame,320,240);
    return 0;
}
static void z26_stop(void){
    spmp_2600_active=0;
    spmp_2600_host=0;
    free(spmp_tia_pool);
    spmp_tia_pool=0;
}
const spmp_core_ops spmp_atari2600_ops={z26_start,z26_tick,z26_stop,NULL,NULL,NULL,NULL};
