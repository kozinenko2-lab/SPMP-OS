/* Host-only instrumentation for Stage 18 validation. Not SPMP-OS HAL ABI. */
#ifndef SPMP_LIBRETRO_STATS_H
#define SPMP_LIBRETRO_STATS_H
#include <stddef.h>
size_t spmp_libretro_rom_capacity(unsigned core_index);
size_t spmp_libretro_frame_bytes(unsigned core_index);
unsigned spmp_libretro_input_polls(unsigned core_index);
unsigned spmp_libretro_skipped_video(unsigned core_index);
#endif
