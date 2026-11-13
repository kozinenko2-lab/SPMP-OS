#ifndef SPMP_PICODRIVE_YM2413_DISABLED_H
#define SPMP_PICODRIVE_YM2413_DISABLED_H
#include <stdint.h>
typedef struct { unsigned placeholder; } OPLL;
OPLL *OPLL_new(uint32_t clock,uint32_t rate);
void OPLL_delete(OPLL *p);
void OPLL_reset(OPLL *p);
void OPLL_setRate(OPLL *p,uint32_t rate);
void OPLL_setChipType(OPLL *p,int chip);
int16_t OPLL_calc(OPLL *p);
#endif
