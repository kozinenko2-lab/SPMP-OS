#include "spmp_mp3_app.h"
#include <string.h>
int spmp_mp3_app_open(spmp_mp3_app *app,const spmp_mp3_io *io,const spmp_multimedia_driver *driver){
 if(!app || !io || !driver || spmp_mp3_player_size()>sizeof(app->arena.bytes))return -1;
 memset(app,0,sizeof(*app));
 app->player=(spmp_mp3_player*)app->arena.bytes;app->volume=256;
 spmp_mp3_stage10_init(&app->sink,driver);
 if(spmp_mp3_open(app->player,io,spmp_mp3_stage10_pcm,&app->sink)!=0)return -1;
 app->active=1;return 0;
}
int spmp_mp3_app_tick(spmp_mp3_app *app,uint32_t keys){
 uint32_t just;int status;
 if(!app||!app->active)return -1;
 just=keys & ~app->prev_keys;app->prev_keys=keys;
 if(just&DEFOS_KEY_SELECT){spmp_mp3_app_close(app);return 0;}
 if(just&DEFOS_KEY_START)spmp_mp3_pause(app->player,!spmp_mp3_paused(app->player));
 if(just&DEFOS_KEY_UP){app->volume=app->volume>240?256:app->volume+16;}
 if(just&DEFOS_KEY_DOWN){app->volume=app->volume<16?0:app->volume-16;}
 spmp_mp3_set_volume(app->player,app->volume);
 if(just&(DEFOS_KEY_LEFT|DEFOS_KEY_RIGHT)){
  uint32_t pos=spmp_mp3_position_ms(app->player);
  if(just&DEFOS_KEY_RIGHT){pos=pos>0xffffffffu-5000?0xffffffffu:pos+5000;}
  else pos=pos>5000?pos-5000:0;
  if(spmp_mp3_seek_ms(app->player,pos)==0){
   spmp_mp3_stage10_close(&app->sink); /* flush playback buffer on seek */
  }
 }
 status=spmp_mp3_step(app->player);
 if(status<=0)spmp_mp3_app_close(app);
 return status;
}
void spmp_mp3_app_close(spmp_mp3_app *app){
 if(!app || !app->active)return;
 spmp_mp3_close(app->player);spmp_mp3_stage10_close(&app->sink);app->active=0;
}
