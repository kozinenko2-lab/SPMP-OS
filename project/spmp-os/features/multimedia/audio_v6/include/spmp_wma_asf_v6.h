/* ASF/WMA robust streamed structural inspect. No WMA PCM decoder included.
 * SPDX-License-Identifier: MIT */
#ifndef SPMP_WMA_ASF_V6_H
#define SPMP_WMA_ASF_V6_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
 void *user;
 int (*read)(void *user,void *dst,unsigned max_bytes);
 int (*seek)(void *user,uint64_t abs_offset);
 int64_t (*size)(void *user);
} spwma_io;
typedef struct {
 uint16_t codec_id,channels,bits_per_sample,block_align;
 uint32_t sample_rate,bitrate_bps,packet_size,stream_number;
 uint64_t packets,duration_ms,preroll_ms,header_bytes,data_offset;
} spwma_info;
/* Read-only ASF header parser: -1 malformed, -2 unsupported codec,
 * -3 I/O error, 0 valid WMA1/WMA2 (NOT decoded). io.seek required.
 * Limits: <=16 MiB ASF header, <=4096 header objects. Caller owns file.
 */
int spwma_probe(spwma_io io,spwma_info *info);
#ifdef __cplusplus
}
#endif
#endif
