/* Optional adapters for MP3 Player v4 and Audio v5. MIT.
 * The base mixer has NO dependency on either decoder package.
 */
#ifndef SPMP_MIX_DECODERS_V6_H
#define SPMP_MIX_DECODERS_V6_H
#include "spmp_mixer_v6.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Directly usable as spmp_mp3_pcm_callback with spmp_mp3_open(...).
 * The producer must call spmp_mp3_step() from foreground to fill the ring.
 * The ring must have capacity for at least one complete MP3 frame (1152).
 */
typedef struct {
 spmix_ring *ring;
 uint32_t expected_rate;
 unsigned expected_channels;
 uint64_t queued_frames;
} spmix_mp3_bridge;
int spmix_mp3_push(void *ctx,const int16_t *pcm,size_t frames,
                   unsigned sample_rate,unsigned channels);
/* Caution: sp_audio_decode can decode internally during pull — call only
 * from foreground/pre-fill task, NEVER from IRQ/DMA interrupt.
 */
struct sp_audio_player;
int spmix_audio_v5_pull(void *ctx,int16_t *dst,unsigned max_frames);
#ifdef __cplusplus
}
#endif
#endif
