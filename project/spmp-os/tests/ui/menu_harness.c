#define _POSIX_C_SOURCE 200809L
#include "spmp_game_menu.h"
#include "spmp_input_hal.h"
#include "spmp_ui_posix.h"
#include "spmp_theme.h"
#include "defos.h"
const char *defos_kind_name(defos_kind kind){(void)kind;return "OTHER";}
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {uint8_t state[40];unsigned pauses,resumes,exits,reads,writes;int fail_save;} fake_core;
static size_t state_size(void *u){(void)u;return 40;}
static int serialize(void *u,void *dst,size_t n){fake_core*c=u;if(n!=40||c->fail_save)return -1;
 memcpy(dst,c->state,n);c->writes++;return 0;}
static int unserialize(void *u,const void *src,size_t n){fake_core*c=u;if(n!=40)return -1;
 memcpy(c->state,src,n);c->reads++;return 0;}
static void pause_game(void *u){((fake_core*)u)->pauses++;}
static void resume_game(void *u){((fake_core*)u)->resumes++;}
static int exit_game(void *u){((fake_core*)u)->exits++;return 0;}

int main(void){
 char dir[]="/tmp/spmp-menu-XXXXXX";assert(mkdtemp(dir));
 spmp_ui_posix fs;assert(!spmp_ui_posix_init(&fs,dir));
 spmp_ui_storage storage=spmp_ui_posix_storage(&fs);
 fake_core core={0};
 for(unsigned i=0;i<40;i++)core.state[i]=(uint8_t)(i+13u);
 spmp_ui_emulator emu={&core,state_size,serialize,unserialize,pause_game,resume_game,NULL,exit_game};
 spmp_game_menu *m=(spmp_game_menu*)calloc(1,sizeof *m);
 uint8_t *scratch=(uint8_t*)malloc(60000);
 uint16_t *screen=(uint16_t*)malloc(320u*240u*2u);
 assert(m&&scratch&&screen);
 spmp_menu_init(m,"nes","/SD/Games/Test.nes",&storage,&emu,scratch,60000);
 spmp_theme t;spmp_theme_default(&t);
 /* Source is 256x240, 120x90 output with black side bars. */
 for(unsigned y=0;y<240;y++)for(unsigned x=0;x<256;x++)screen[y*256+x]=(uint16_t)(x^y);
 assert(!spmp_menu_capture_clean(m,screen,256,240,512));
 uint16_t expected= m->clean_frame[SPMP_UI_THUMB_W*20+20];
 assert(m->clean_valid && expected);
 /* No early menu on key down, short press on release. */
 assert(spmp_menu_input(m,SPMP_UI_POWER,1000)==0);
 assert(!m->opened);
 assert(spmp_menu_input(m,0,1160)==1 && m->opened&&core.pauses==1);
 spmp_menu_draw(m,&t,screen,320,240);
 assert(m->clean_frame[SPMP_UI_THUMB_W*20+20]==expected);
 /* save: select via down, press confirm */
 assert(spmp_menu_input(m,SPMP_UI_DOWN,1200)==0);
 assert(m->selection==SPMP_UI_SAVE);
 assert(spmp_menu_input(m,0,1210)==0);
 assert(spmp_menu_input(m,SPMP_UI_CONFIRM,1220)==2);
 assert(core.writes==1 && m->preview_valid);
 assert(!memcmp(m->preview,m->clean_frame,sizeof m->preview));
 char path0[SPMP_UI_PATH_MAX];assert(!spmp_menu_save_path(m,path0,sizeof path0));
 char stored[512];snprintf(stored,sizeof stored,"%s/%s",dir,path0);
 FILE *f=fopen(stored,"rb");assert(f);
 uint8_t magic[8];assert(fread(magic,1,8,f)==8&&!memcmp(magic,"SPMPSAV1",8));
 fclose(f);
 /* Change emulator RAM, then load via menu selection. */
 memset(core.state,0,sizeof core.state);
 assert(!spmp_menu_input(m,0,1230));
 assert(!spmp_menu_input(m,SPMP_UI_DOWN,1240));
 assert(m->selection==SPMP_UI_LOAD);
 assert(!spmp_menu_input(m,0,1250));
 assert(spmp_menu_input(m,SPMP_UI_CONFIRM,1260)==3);
 for(unsigned i=0;i<40;i++)assert(core.state[i]==i+13u);
 assert(core.reads==1);
 /* 2nd slot is empty, no false success; game state retained. */
 assert(!spmp_menu_input(m,0,1270));
 assert(!spmp_menu_input(m,SPMP_UI_RIGHT,1280));
 assert(m->slot==1&&!m->preview_valid);
 assert(!spmp_menu_input(m,0,1290));
 assert(spmp_menu_input(m,SPMP_UI_CONFIRM,1300)==-1);
 assert(core.state[0]==13 && core.reads==1);
 /* Slot 1 save is independent. */
 assert(!spmp_menu_input(m,0,1310));
 assert(!spmp_menu_input(m,SPMP_UI_UP,1320));
 assert(m->selection==SPMP_UI_SAVE);
 assert(!spmp_menu_input(m,0,1330));
 core.state[0]=77;
 assert(spmp_menu_input(m,SPMP_UI_CONFIRM,1340)==2);
 char path1[SPMP_UI_PATH_MAX];assert(!spmp_menu_save_path(m,path1,sizeof path1));
 assert(strcmp(path0,path1));
 /* Corrupt a committed save; preview validation rejects it. */
 char save1[512];snprintf(save1,sizeof save1,"%s/%s",dir,path1);
 f=fopen(save1,"r+b");assert(f);assert(!fseek(f,SPMP_UI_STATE_HEADER+20,SEEK_SET));
 int c=fgetc(f);assert(c!=EOF);assert(!fseek(f,-1,SEEK_CUR));assert(fputc(c^1,f)!=EOF);fclose(f);
 assert(spmp_menu_refresh_preview(m)==-1 && !m->preview_valid);
 core.state[0]=0;
 assert(spmp_menu_load(m)==-1 && core.state[0]==0);
 /* Failed save never overwrites slot 0 or slot 1. */
 core.fail_save=1;
 assert(spmp_menu_save(m)==-1);
 core.fail_save=0;
 assert(!spmp_menu_input(m,0,1400));
 assert(!spmp_menu_input(m,SPMP_UI_LEFT,1410));
 assert(m->slot==0 && m->preview_valid);
 assert(m->preview[20*SPMP_UI_THUMB_W+20]==expected);
 /* Returning to game must not remove the active emulator. */
 assert(spmp_menu_input(m,SPMP_UI_POWER,1600)==0);
 assert(spmp_menu_input(m,0,1750)==1&&!m->opened&&core.resumes==1);
 /* Long press (>650 ms) is reserved for hardware power shutdown. */
 assert(spmp_menu_input(m,SPMP_UI_POWER,2000)==0);
 assert(spmp_menu_input(m,SPMP_UI_POWER,3200)==0);
 assert(spmp_menu_input(m,0,3250)==0 && !m->opened);
 /* Settings is a submenu of every emulator, not a new savestate slot. */
 assert(spmp_menu_input(m,SPMP_UI_POWER,3500)==0);
 assert(spmp_menu_input(m,0,3650)==1&&m->opened);
 for(unsigned i=0;i<4;i++){
   assert(!spmp_menu_input(m,SPMP_UI_DOWN,3700+i*20));
   assert(!spmp_menu_input(m,0,3710+i*20));
 }
 assert(m->selection==SPMP_UI_SETTINGS);
 assert(spmp_menu_input(m,SPMP_UI_CONFIRM,3790)==1&&m->settings_open);
 assert(m->settings.count>=4);
 assert(!spmp_menu_input(m,0,3810));
 assert(!spmp_menu_input(m,SPMP_UI_DOWN,3820));
 int oldvol=m->settings.values[1];
 assert(!spmp_menu_input(m,0,3830));
 assert(!spmp_menu_input(m,SPMP_UI_RIGHT,3840));
 assert(m->settings.values[1]!=oldvol);
 assert(!spmp_menu_input(m,0,3850));
 assert(spmp_menu_input(m,SPMP_UI_BACK,3860)==1&&!m->settings_open);
 assert(spmp_menu_input(m,SPMP_UI_POWER,3900)==0);
 assert(spmp_menu_input(m,0,3980)==1&&!m->opened);
 /* exit path */
 assert(!spmp_menu_input(m,SPMP_UI_POWER,4000));
 assert(spmp_menu_input(m,0,4120)==1);
 for(unsigned i=0;i<5;i++){
  assert(!spmp_menu_input(m,SPMP_UI_DOWN,4200+i*30));
  assert(!spmp_menu_input(m,0,4210+i*30));
 }
 assert(m->selection==SPMP_UI_EXIT);
 assert(spmp_menu_input(m,SPMP_UI_CONFIRM,4390)==4 && core.exits==1 && m->quit);
 /* Raw masks: firmware libgame maps raw -> normalized; Power is unknown by default. */
 spmp_hw_keymap map={0};map.raw[SPMP_HW_UP]=1u;map.raw[SPMP_HW_O]=1u<<16;
 assert(spmp_hw_translate(&map,(1u<<16)|1u)==(SPMP_UI_CONFIRM|SPMP_UI_UP));
 assert(!(spmp_hw_translate(&map,1u<<13)&SPMP_UI_POWER));
 assert(spmp_hw_unmapped(&map,1u<<13)==(1u<<13));
 map.power_raw_mask=1u<<20; /* SIMULATED ONLY! not Defender bit assignment */
 assert(spmp_hw_translate(&map,1u<<20)==SPMP_UI_POWER);
 /* Different ROM must use different storage path, even with same core. */
 spmp_game_menu other;
 spmp_menu_init(&other,"nes","/SD/Games/Other.nes",&storage,&emu,scratch,60000);
 char other_path[SPMP_UI_PATH_MAX];assert(!spmp_menu_save_path(&other,other_path,sizeof other_path));
 assert(strcmp(path0,other_path));
 free(screen);free(scratch);free(m);
 printf("SPMP menu: power tap/hold, all 5 actions, atomic state+thumbnail, 5 slots, checksum, crash-safe rejection OK\n");
 return 0;
}
