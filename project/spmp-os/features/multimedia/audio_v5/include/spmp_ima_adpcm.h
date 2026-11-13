#ifndef SPMP_IMA_ADPCM_H
#define SPMP_IMA_ADPCM_H
#include <stddef.h>
#include <stdint.h>
/* Decode a Microsoft WAVE IMA ADPCM block. PCM capacity counts interleaved int16_t.
 * Returns frames decoded or negative on invalid data. Stereo is arranged in
 * alternating groups of four bytes per channel according to RIFF IMA WAVE. */
int sp_ima_decode_block(const uint8_t *src,size_t bytes,unsigned channels,
                        int16_t *dst,size_t pcm_capacity);
#endif
