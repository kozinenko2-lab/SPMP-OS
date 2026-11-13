#ifndef SPMP_UI_ROUTER_H
#define SPMP_UI_ROUTER_H
#include "spmp_game_menu.h"
#include "spmp_cores.h"
/* Wrap present callback for ALL cores without changing their implementations.
 * Each frame is captured from untouched core RGB565 pixels first. */
typedef struct {
 spmp_game_menu *menu;
 spmp_core_host underlying;
 spmp_core_host wrapped;
} spmp_ui_router;
int spmp_ui_wrap_host(spmp_ui_router *r,spmp_game_menu *menu,
                      const spmp_core_host *underlying);
#endif
