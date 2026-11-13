#ifndef SPMP_RUNTIME_H
#define SPMP_RUNTIME_H
#include "spmp_overlay.h"
#include "spmp_memory.h"
#define SPMP_RUNTIME_GAME_ID_MAX 128u
/* This is the portable code/ROM residency coordinator. It does NOT call into
 * ARM machine code on the host. The device HAL must supply verified RAM/cache. */
typedef enum {SPMP_RUN_EMPTY=0,SPMP_RUN_CORE_READY=1,
 SPMP_RUN_GAME=2,SPMP_RUN_PAUSED=3} spmp_run_state;
typedef int (*spmp_before_game_change)(void *ctx,
 const char *core_id,const char *game_id);
typedef struct {
 spmp_overlay_manager loader;
 spmp_arena game;
 spmp_run_state state;
 char game_id[SPMP_RUNTIME_GAME_ID_MAX];
 size_t rom_length;
 unsigned rom_changes;
 spmp_before_game_change save;
 void *save_ctx;
} spmp_runtime;
void spmp_runtime_init(spmp_runtime *rt,void *code,size_t code_size,
 void *game,size_t game_size,const spmp_overlay_options *options,
 spmp_before_game_change save,void *save_ctx);
/* 0=new ROM, 1=resume. ROM bytes must live OUTSIDE the game arena. */
int spmp_runtime_launch(spmp_runtime *rt,const uint8_t *elf,size_t elf_size,
 const char *game_id,const uint8_t *rom,size_t rom_bytes);
int spmp_runtime_pause(spmp_runtime *rt);
int spmp_runtime_resume(spmp_runtime *rt);
int spmp_runtime_stop_game(spmp_runtime *rt);
int spmp_runtime_shutdown(spmp_runtime *rt);
#endif
