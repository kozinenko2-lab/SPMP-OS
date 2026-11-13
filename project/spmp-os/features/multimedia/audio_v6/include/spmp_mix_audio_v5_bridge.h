/* Large-frame buffered adapter for Audio v5 codecs (especially FLAC).
 * Supply caller-owned PCM workspace >=max_block_frames*channels*2 for FLAC.
 * This is *in addition* to the Audio v5 compressed/workspace allocation.
 */
#ifndef SPMP_MIX_AUDIO_V5_BRIDGE_H
#define SPMP_MIX_AUDIO_V5_BRIDGE_H
#include "spmp_mixer_v6.h"
#include "spmp_audio_v5.h"
typedef struct {
 sp_audio_player *player;
 int16_t *pcm;
 unsigned capacity_frames,channels;
 unsigned offset_frames, buffered_frames;
 unsigned decoder_errors;
} spmix_audio_v5_buffered;
int spmix_audio_v5_buffered_init(spmix_audio_v5_buffered *ctx,sp_audio_player *player,
                                 int16_t *buffer,unsigned buffer_frames,unsigned channels);
int spmix_audio_v5_buffered_pull(void *ctx,int16_t *dst,unsigned max_frames);
#endif
