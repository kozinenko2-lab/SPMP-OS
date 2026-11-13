#ifndef SPMP_AMR_OPENCORE_H
#define SPMP_AMR_OPENCORE_H
#include "spmp_audio_v5.h"
/* Optional OpenCORE AMR-NB / AMR-WB backend. This adapter does not redistribute
 * OpenCORE, and requires a separately supplied native decoder library. */
typedef struct {
 sp_audio_io io;
 void *decoder;
 int type,at_eof;
 int16_t samples[320];
 unsigned sample_count,sample_offset;
 uint64_t frame_index;
} sp_amr_opencore_ctx;
void sp_amr_opencore_make(sp_audio_external_backend *out,sp_amr_opencore_ctx *ctx,int format);
#endif
