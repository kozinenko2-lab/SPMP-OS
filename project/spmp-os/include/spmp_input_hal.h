#ifndef SPMP_INPUT_HAL_H
#define SPMP_INPUT_HAL_H
#include <stdint.h>
#include "spmp_game_menu.h"

/* libgame emuIfKeyInit maps logical keys to raw scancodes per device.
 * POWER is deliberately left unassigned until identified on actual Defender.
 * Safe even when a power event is consumed by PMIC/firmware before raw poll. */
enum { SPMP_HW_UP=0,SPMP_HW_DOWN=1,SPMP_HW_LEFT=2,SPMP_HW_RIGHT=3,
 SPMP_HW_O=4,SPMP_HW_X=5,SPMP_HW_SQUARE=6,SPMP_HW_TRIANGLE=7,
 SPMP_HW_R=8,SPMP_HW_L=9,SPMP_HW_SELECT=10,SPMP_HW_START=11,
 SPMP_HW_ESC=12 };
typedef struct {uint32_t raw[20];uint32_t power_raw_mask;uint8_t power_active_low;} spmp_hw_keymap;
/* Caller supplies an experimentally confirmed power mask; zero disables it. */
uint32_t spmp_hw_translate(const spmp_hw_keymap *map,uint32_t raw);
/* Collect raw unrecognized bits for diagnostics; no guesses about meaning. */
uint32_t spmp_hw_unmapped(const spmp_hw_keymap *map,uint32_t raw);
#endif
