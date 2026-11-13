#include "spmp_runtime.h"
#include <string.h>
#include <stdint.h>
static int same_string(const char *a,const char *b){return strcmp(a,b)==0;}
static int save_game(spmp_runtime *rt){
 if(rt->state!=SPMP_RUN_GAME&&rt->state!=SPMP_RUN_PAUSED)return 0;
 return rt->save?rt->save(rt->save_ctx,rt->loader.resident.meta.core_id,rt->game_id):0;
}
void spmp_runtime_init(spmp_runtime *rt,void *code,size_t code_size,
 void *game,size_t game_size,const spmp_overlay_options *options,
 spmp_before_game_change save,void *save_ctx){
 if(!rt)return;
 memset(rt,0,sizeof *rt);
 spmp_overlay_manager_init(&rt->loader,code,code_size,options);
 spmp_arena_init(&rt->game,game,game_size);
 rt->save=save;rt->save_ctx=save_ctx;
}
int spmp_runtime_launch(spmp_runtime *rt,const uint8_t *elf,size_t elf_size,
 const char *game_id,const uint8_t *rom,size_t rom_bytes){
 if(!rt||!game_id||!rom||!rom_bytes)return SPMP_OV_BAD_INPUT;
 size_t n=strlen(game_id);
 if(!n||n>=SPMP_RUNTIME_GAME_ID_MAX)return SPMP_OV_BAD_INPUT;
 if(rom_bytes>rt->game.capacity)return SPMP_OV_NO_MEMORY;
 uintptr_t ga=(uintptr_t)rt->game.memory,ra=(uintptr_t)rom;
 /* Forbid source ROM overlapping memory about to be zeroed. */
 if(!rt->game.memory||ga>UINTPTR_MAX-rt->game.capacity ||
    ra>UINTPTR_MAX-rom_bytes ||
    (ra<ga+rt->game.capacity && ga<ra+rom_bytes))return SPMP_OV_BAD_INPUT;
 spmp_overlay_image candidate;
 int rc=spmp_overlay_probe(elf,elf_size,rt->loader.slot_size,&rt->loader.options,&candidate);
 if(rc)return rc;
 if(rom_bytes>candidate.meta.max_game_bytes)return SPMP_OV_NO_MEMORY;
 if(rt->loader.loaded && rt->state==SPMP_RUN_PAUSED &&
    rt->loader.resident.source_fingerprint==candidate.source_fingerprint &&
    same_string(rt->loader.resident.meta.core_id,candidate.meta.core_id) &&
    same_string(rt->game_id,game_id)){
  rt->state=SPMP_RUN_GAME;return 1;
 }
 if(save_game(rt))return SPMP_OV_NOT_READY;
 rc=spmp_overlay_manager_switch(&rt->loader,elf,elf_size,NULL);
 if(rc<0){
  /* Old ROM was saved; leave its state untouched on a rejected switch. */
  return rc;
 }
 spmp_arena_reset(&rt->game);
 void *dst=spmp_arena_alloc(&rt->game,rom_bytes,16u);
 if(!dst){rt->state=SPMP_RUN_CORE_READY;rt->game_id[0]=0;rt->rom_length=0;return SPMP_OV_NO_MEMORY;}
 memcpy(dst,rom,rom_bytes);
 memcpy(rt->game_id,game_id,n+1);
 rt->rom_length=rom_bytes;
 rt->state=SPMP_RUN_GAME;
 rt->rom_changes++;
 return 0;
}
int spmp_runtime_pause(spmp_runtime *rt){
 if(!rt||rt->state!=SPMP_RUN_GAME)return -1;
 rt->state=SPMP_RUN_PAUSED;return 0;
}
int spmp_runtime_resume(spmp_runtime *rt){
 if(!rt||rt->state!=SPMP_RUN_PAUSED)return -1;
 rt->state=SPMP_RUN_GAME;return 0;
}
int spmp_runtime_stop_game(spmp_runtime *rt){
 if(!rt)return -1;
 if(save_game(rt))return -1;
 spmp_arena_reset(&rt->game);rt->rom_length=0;rt->game_id[0]=0;
 rt->state=rt->loader.loaded?SPMP_RUN_CORE_READY:SPMP_RUN_EMPTY;
 return 0;
}
int spmp_runtime_shutdown(spmp_runtime *rt){
 if(!rt)return -1;
 if(spmp_runtime_stop_game(rt))return -1;
 spmp_overlay_manager_unload(&rt->loader);
 rt->state=SPMP_RUN_EMPTY;return 0;
}
