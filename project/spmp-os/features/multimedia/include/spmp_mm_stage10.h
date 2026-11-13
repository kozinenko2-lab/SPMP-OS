/* Optional glue to Stage10 public HAL, not an emulator dependency. */
#ifndef SPMP_MM_STAGE10_H
#define SPMP_MM_STAGE10_H
#include "spmp_mm.h"
#include "defos_hal.h"
typedef struct {
 const defos_hal *platform;
} mm_stage10_audio;
/* The caller owns adapter and HAL. No device implementation is implied. */
void mm_stage10_bind_audio(mm_audio_output *dst,mm_stage10_audio *adapter,
                           const defos_hal *platform);
#endif
