#ifndef SPMP_MIX_STAGE10_V6_H
#define SPMP_MIX_STAGE10_V6_H
#include "spmp_mixer_v6.h"
#include "spmp_multimedia_hal.h"
typedef struct {
 const spmp_multimedia_driver *hal;
 int active;
} spmix_stage10_context;
/* sink is stereo output; decoder and mixer run in the foreground task. */
void spmix_bind_stage10(spmix_sink *sink,spmix_stage10_context *ctx,
                        const spmp_multimedia_driver *hal);
#endif
