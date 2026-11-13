/* SPMP-OS limited Mega Drive-only profile. NO_SMS required. */
#include <stddef.h>
#include <stdint.h>
#include "ym2413.h"
OPLL *opll = NULL;
static OPLL unused_opll;
OPLL *OPLL_new(uint32_t clock,uint32_t rate) {(void)clock;(void)rate;return &unused_opll;}
void OPLL_delete(OPLL *p) {(void)p;}
void OPLL_reset(OPLL *p) {(void)p;}
void OPLL_setRate(OPLL *p,uint32_t rate) {(void)p;(void)rate;}
void OPLL_setChipType(OPLL *p,int chip) {(void)p;(void)chip;}
int16_t OPLL_calc(OPLL *p) {(void)p;return 0;}
void YM2413_regWrite(unsigned v) {(void)v;}
void YM2413_dataWrite(unsigned v) {(void)v;}
size_t ym2413_pack_state(void *buf,size_t size) {(void)buf;(void)size;return 0;}
void ym2413_unpack_state(const void *buf,size_t size) {(void)buf;(void)size;}
