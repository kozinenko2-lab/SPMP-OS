#ifndef SPMP_MP3_INTERNAL_H
#define SPMP_MP3_INTERNAL_H
#include "spmp_mp3_player.h"
#include "stream.h"
#include "frame.h"
#include "synth.h"

struct spmp_mp3_player {
 spmp_mp3_io io;
 spmp_mp3_pcm_callback on_pcm;
 void *pcm_ctx;
 struct mad_stream stream;
 struct mad_frame frame;
 struct mad_synth synth;
 uint8_t data[SPMP_MP3_IOBUF + MAD_BUFFER_GUARD];
 uint8_t main_data[MAD_BUFFER_MDLEN];
 mad_fixed_t overlap[2][32][18];
 int16_t pcm[SPMP_MP3_MAX_PCM_FRAMES*2];
 size_t valid;
 uint64_t base_offset; /* beginning of data[] in file */
 uint64_t next_offset; /* next read byte */
 spmp_mp3_metadata meta;
 uint8_t xing_toc[100];
 uint32_t xing_bytes;
 uint32_t xing_frames;
 uint32_t trim_start, trim_end;
 uint64_t output_samples, raw_samples;
 uint32_t suppress_until_ms;
 uint32_t duration_ms;
 uint32_t position_ms;
 uint64_t samples_total;
 uint32_t bad;
 unsigned volume;
 int have_toc, eof, finished, paused, initialized, first_frame, valid_audio;
};
#endif
