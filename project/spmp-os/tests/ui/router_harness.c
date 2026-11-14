#include "spmp_ui_router.h"
#include "spmp_hw_adapter.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int frames,audio_calls,init_called,close_called;
static uint32_t held_keys=15;
static int16_t latest_audio;
static void raw_present(void *ctx,const uint16_t*pix,unsigned w,unsigned h){(void)ctx;assert(pix&&w==160&&h==144);frames++;}
static void raw_audio(void *ctx,const int16_t *pcm,size_t n,unsigned r){(void)ctx;assert(pcm&&n&&r);latest_audio=pcm[0];audio_calls++;}
static uint32_t keys(void *ctx){(void)ctx;return held_keys;}
static int size_query(void *ctx,const char *path,size_t *bytes){assert(ctx==&held_keys&&!strcmp(path,"nested/game.a26"));*bytes=4096;return 0;}
static int read_entire(void *ctx,const char *path,uint8_t *buf,size_t cap,size_t *got){
 (void)ctx;(void)path;if(cap<4)return -1;memset(buf,0x33,4);*got=4;return 0;
}
static int init(spmp_legacy_keymap *m){init_called++;m->scancode[SPMP_HW_UP]=1u;
 m->scancode[SPMP_HW_O]=1u<<16;return 0;}
static uint32_t poll(spmp_legacy_keymap *m){(void)m;return 1u|(1u<<16)|(1u<<20);}
static int cleanup(spmp_legacy_keymap *m){(void)m;close_called++;return 0;}
int main(void){
 spmp_game_menu menu;spmp_menu_init(&menu,"gb","test.gb",NULL,NULL,NULL,0);
 spmp_core_host h={0};h.user=NULL;h.present=raw_present;h.buttons=keys;h.read_entire=read_entire;h.audio_pcm=raw_audio;
 spmp_ui_router r;assert(!spmp_ui_wrap_host(&r,&menu,&h));
 assert(!r.wrapped.file_size);
 h.user=&held_keys;h.file_size=size_query;assert(!spmp_ui_wrap_host(&r,&menu,&h));
 size_t bytes=0;assert(r.wrapped.file_size&&!r.wrapped.file_size(r.wrapped.user,"nested/game.a26",&bytes)&&bytes==4096);
 uint16_t img[160u*144u];for(size_t i=0;i<160u*144u;i++)img[i]=(uint16_t)(i+2u);
 assert(r.wrapped.buttons(r.wrapped.user)==15);
 r.wrapped.present(r.wrapped.user,img,160,144);
 assert(frames==1&&menu.clean_valid);
 uint16_t keep=menu.clean_frame[500];
 int16_t pcm[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 /* Default volume 8/10 now actually scales samples. */
 r.wrapped.audio_pcm(r.wrapped.user,pcm,4,44100);
 assert(audio_calls==1&&latest_audio==8000);
 menu.settings.values[1]=5; /* Simulated persisted setting. */
 r.wrapped.audio_pcm(r.wrapped.user,pcm,4,44100);
 assert(audio_calls==2&&latest_audio==5000);
 menu.settings.values[2]=1;held_keys=(1u<<4);
 assert(r.wrapped.buttons(r.wrapped.user)==(1u<<5));
 menu.settings.values[2]=0;held_keys=15;
 menu.opened=1;
 assert(!r.wrapped.buttons(r.wrapped.user));
 r.wrapped.present(r.wrapped.user,img,160,144);
 assert(frames==2&&menu.clean_frame[500]==keep);
 r.wrapped.audio_pcm(r.wrapped.user,pcm,4,44100);assert(audio_calls==2);
 spmp_legacy_input in;
 assert(!spmp_legacy_input_init(&in,init,poll,cleanup));
 assert(init_called==1);
 assert(spmp_legacy_buttons(&in)==(SPMP_UI_UP|SPMP_UI_CONFIRM));
 spmp_legacy_confirm_power(&in,1u,0); /* overlaps D-pad: rejected */
 assert(!in.mapping.power_raw_mask);
 spmp_legacy_confirm_power(&in,1u<<20,0);
 assert(spmp_legacy_buttons(&in)==(SPMP_UI_UP|SPMP_UI_CONFIRM|SPMP_UI_POWER));
 spmp_legacy_input_close(&in);assert(close_called==1);
 printf("SPMP input HAL: legacy raw masks, unassigned Power, video frame snapshot, muted UI audio OK\n");
 return 0;
}
