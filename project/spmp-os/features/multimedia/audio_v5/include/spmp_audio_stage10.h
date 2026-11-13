#ifndef SPMP_AUDIO_STAGE10_H
#define SPMP_AUDIO_STAGE10_H
#include "spmp_audio_v5.h"
#include "spmp_multimedia_hal.h"
typedef struct {
 const spmp_multimedia_driver *hal;
 unsigned rate, channels;
 int active;
} sp_audio_stage10_ctx;
void sp_audio_stage10_bind(sp_audio_sink *out,sp_audio_stage10_ctx *ctx,
                           const spmp_multimedia_driver *hal);
#endif
