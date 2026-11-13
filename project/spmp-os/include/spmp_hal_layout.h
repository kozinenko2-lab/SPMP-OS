#ifndef SPMP_HAL_LAYOUT_H
#define SPMP_HAL_LAYOUT_H
#include <stdint.h>
#include <stddef.h>
/* All addresses are DEVICE physical RAM addresses, never host pointers.
 * This validates a planned layout; it does not discover actual Defender RAM. */
typedef struct {uint32_t start,size;} spmp_phys_region;
typedef struct {
 spmp_phys_region total;
 spmp_phys_region redboot_reserved;
 spmp_phys_region os;
 spmp_phys_region core_slot;
 spmp_phys_region game;
 spmp_phys_region video;
 spmp_phys_region audio;
} spmp_hal_layout;
/* Guard against integer wraps, overlaps and RedBoot-owned regions. */
int spmp_hal_check_layout(const spmp_hal_layout *layout);
/* Once real device memory is discovered, this may be passed to Stage10
 * overlay options. NEVER infer a physical address from a host pointer. */
#endif
