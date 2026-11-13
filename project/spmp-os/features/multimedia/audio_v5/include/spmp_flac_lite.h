#ifndef SPMP_FLAC_LITE_H
#define SPMP_FLAC_LITE_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Standalone, bounded-memory FLAC frame decoder: native FLAC, mono/stereo,
   8/12/16/20/24 bits; no malloc, no filesystem, no float, no libc decode dependency.
   A compressed file may contain features outside this implementation; report
   SPFL_UNSUPPORTED rather than claiming full FLAC standard compatibility. */
enum { SPFL_OK=0, SPFL_EOF=1, SPFL_BAD=-1, SPFL_UNSUPPORTED=-2, SPFL_SMALL=-3, SPFL_CRC=-4 };
typedef struct spmp_flac {
 const uint8_t *data;
 size_t size, first_frame, cursor;
 uint32_t sample_rate;
 uint32_t max_block;
 uint64_t total_samples;
 uint8_t channels, bits_per_sample;
} spmp_flac;
int spmp_flac_open(spmp_flac *d, const uint8_t *data, size_t size);
void spmp_flac_rewind(spmp_flac *d);
size_t spmp_flac_work_size(uint32_t max_block);
/* scratch holds 2 * max_block int32_t samples, aligned to 4 bytes.
   capacity is number of int16_t, not bytes; out_frames receives PCM frame count. */
int spmp_flac_decode_frame(spmp_flac *d, void *scratch, size_t scratch_bytes,
                           int16_t *interleaved, size_t capacity,
                           size_t *out_frames);
/* Decode from a caller-managed compressed window (SD streaming).
   Input must contain at least one complete FLAC frame; on success returns
   consumed bytes. No whole-file compressed buffer is required. */
int spmp_flac_decode_chunk(const spmp_flac *meta,
                           const uint8_t *compressed, size_t compressed_bytes,
                           void *scratch, size_t scratch_bytes,
                           int16_t *interleaved, size_t capacity,
                           size_t *consumed, size_t *out_frames);
uint8_t spmp_flac_crc8(const uint8_t *data, size_t len);
uint16_t spmp_flac_crc16(const uint8_t *data, size_t len);
#ifdef __cplusplus
}
#endif
#endif
