#include "spmp_game_menu.h"
#include "spmp_cores.h"
#include "spmp_settings.h"
static size_t core_state_size(void *u){(void)u;return spmp_core_state_size();}
static int core_save(void *u,void *p,size_t n){(void)u;return spmp_core_serialize(p,n);}
static int core_load(void *u,const void *p,size_t n){(void)u;return spmp_core_unserialize(p,n);}
static void core_pause(void *u){(void)u;(void)spmp_core_pause_to_menu();}
static void core_resume(void *u){(void)u;(void)spmp_core_resume();}
static int core_option(void*u,spmp_setting_id id,int value){(void)u;
 if(id==SPMP_OPT_VOLUME||id==SPMP_OPT_SWAP_AB)return 0;
 return spmp_core_apply_setting((unsigned)id,value);
}
static int core_exit(void *u){(void)u;spmp_core_stop();return 0;}
spmp_ui_emulator spmp_game_core_emulator(void){
 spmp_ui_emulator e={NULL,core_state_size,core_save,core_load,core_pause,core_resume,core_option,core_exit};
 return e;
}
