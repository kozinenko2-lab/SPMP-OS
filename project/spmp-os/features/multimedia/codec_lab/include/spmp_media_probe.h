#ifndef SPMP_MEDIA_PROBE_H
#define SPMP_MEDIA_PROBE_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Pure metadata/demux helpers; no actual MP3/AAC PCM decode. */
typedef struct spmp_mp3_frame {
 uint32_t sample_rate, bit_rate_bps, frame_bytes, samples_per_frame;
 uint8_t channels, layer, mpeg_version;
} spmp_mp3_frame;
typedef struct spmp_aac_adts {
 uint32_t sample_rate, frame_bytes, samples_per_frame;
 uint8_t channels, object_type, header_bytes;
} spmp_aac_adts;
/* Returns 0 on valid header. */
int spmp_mp3_header(const uint8_t *p,size_t len,spmp_mp3_frame *out);
int spmp_mp3_find_frame(const uint8_t *p,size_t len,size_t *offset,spmp_mp3_frame *out);
int spmp_aac_adts_header(const uint8_t *p,size_t len,spmp_aac_adts *out);
/* Compatibility research: recovered from original LIBMP3.O, LIBFLAC.O.
   NOT interchangeable with full original decoder ABI or PCM decoding. */
int spmp_stock_mp3_working_ram_recovered(uint32_t available,uint32_t *a,uint32_t *b);
uint32_t spmp_stock_flac_working_ram_recovered(void);
/* Untrusted stock codec IMG header; validated without loading executable bytes. */
typedef struct spmp_mmpi {
 uint32_t total_size, header_tag, header_value, block_record_size;
 uint8_t module_family, module_slot;
} spmp_mmpi;
int spmp_mmpi_inspect(const uint8_t *bytes,size_t len,spmp_mmpi *out);
#ifdef __cplusplus
}
#endif
#endif
