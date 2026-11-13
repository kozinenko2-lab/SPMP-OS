/* Native integration check: HuC6280 really writes work RAM after TAM mapping.
 * Synthetic 8 KiB HuCard, no commercial ROM or BIOS required.
 */
#include "spmp_cores.h"
#include "shared.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static uint8_t hu_card[0x2000];
static unsigned present_count,audio_count;
static unsigned bad_frame;
static void present(void *u,const uint16_t *pixels,unsigned w,unsigned h){
    (void)u;
    present_count++;
    if(!pixels||w!=320||h!=240)bad_frame++;
}
static uint32_t buttons(void *u){(void)u;return 0;}
static int read_all(void *u,const char *path,uint8_t *dst,size_t cap,size_t *len){
    (void)u;
    if(!path||strcmp(path,"synthetic.pce")||cap<sizeof hu_card||!len)return -1;
    memcpy(dst,hu_card,sizeof hu_card);
    *len=sizeof hu_card;
    return 0;
}
static void audio(void *u,const int16_t *pcm,size_t n,unsigned rate){
    (void)u;
    if(!pcm||n<300||rate!=22050)bad_frame++;
    audio_count++;
}
int main(void){
    memset(hu_card,0xea,sizeof hu_card); /* 65xx NOP */
    /* LDA #$F8, TAM #$01 maps 0x0000-0x1fff to work RAM;
       LDA #$42, STA $10, JMP $E008 loops forever at defined address. */
    const uint8_t code[]={0xa9,0xf8,0x53,0x01,0xa9,0x42,0x85,0x10,0x4c,0x08,0xe0};
    memcpy(hu_card,code,sizeof code);
    hu_card[0x1ffe]=0x00;hu_card[0x1fff]=0xe0;
    spmp_core_host host={0};
    host.present=present;host.buttons=buttons;host.read_entire=read_all;host.audio_pcm=audio;
    if(spmp_core_start("pcengine",&host,"synthetic.pce"))return 10;
    if(spmp_core_tick()||spmp_core_tick())return 11;
    if(ram[0x10]!=0x42){fprintf(stderr,"CPU did not execute store, RAM=%02x\n",ram[0x10]);return 12;}
    if(present_count!=2||audio_count!=2||bad_frame)return 13;
    spmp_core_stop();
    /* A second launch must reuse resources without leaving the first state active. */
    if(spmp_core_start("pcengine",&host,"synthetic.pce"))return 14;
    if(spmp_core_tick())return 15;
    if(ram[0x10]!=0x42||present_count!=3||audio_count!=3||bad_frame)return 16;
    spmp_core_stop();
    puts("PC Engine native core: HuC6280, TAM, RAM store, 3 frames, audio, relaunch OK");
    return 0;
}
