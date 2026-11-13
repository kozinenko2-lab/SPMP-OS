#include "spmp_hal_layout.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 /* FAKE 32-MiB plan for allocator tests, not the actual Defender memory map. */
 spmp_hal_layout p={0};
 p.total=(spmp_phys_region){0x80000000u,0x02000000u};
 p.redboot_reserved=(spmp_phys_region){0x80000000u,0x00100000u};
 p.os=(spmp_phys_region){0x80100000u,0x00200000u};
 p.core_slot=(spmp_phys_region){0x80300000u,0x00600000u};
 p.game=(spmp_phys_region){0x80900000u,0x00e00000u};
 p.video=(spmp_phys_region){0x81700000u,0x00200000u};
 p.audio=(spmp_phys_region){0x81900000u,0x00100000u};
 assert(!spmp_hal_check_layout(&p));
 p.game.start=0x80700000u;assert(spmp_hal_check_layout(&p));
 p.game.start=0x80900000u;
 p.core_slot.start=0x80000000u;assert(spmp_hal_check_layout(&p));
 p.core_slot.start=0x80300000u;
 p.total.start=0xfffffff0u;assert(spmp_hal_check_layout(&p));
 p.total.start=0x80000000u;
 p.video.size=0;assert(spmp_hal_check_layout(&p));
 printf("SPMP HAL memory plan: 5 regions, RedBoot reservation, overlaps/overflow checks OK\n");
 return 0;
}
