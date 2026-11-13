/* SPMP-OS independent audio mixer v6 — MIT (2026), no FPU or heap.
 * Run from a foreground audio task, not inside DMA IRQ. Feed PCM before rendering.
 * PCM16 interleaved; callback returns frames >0, 0=end, -2=temporarily empty,
 * <0 other than -2=permanent read error. Callback must never exceed requested frames.
 */
#ifndef SPMP_MIXER_V6_H
#define SPMP_MIXER_V6_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define SPMIX_ABI_VERSION 6u
#define SPMIX_MAX_VOICES 4u
#define SPMIX_FETCH_FRAMES 256u
#define SPMIX_STARVED (-2)
#define SPMIX_ERR (-1)

typedef int (*spmix_pull_fn)(void *ctx, int16_t *interleaved, unsigned max_frames);

typedef struct {
 spmix_pull_fn pull;
 void *user;
 uint32_t input_rate;
 uint64_t phase_q32, step_q32;
 int32_t gain_q16, fade_step_q16;
 uint32_t fade_left;
 uint16_t target_gain_q8;
 int16_t pan; /* -128=left, 0=center, +128=right; constant-power not promised */
 uint8_t channels, active, have_a, have_b, terminal;
 int16_t a[2], b[2];
 unsigned cache_index, cache_length;
 int16_t cache[SPMIX_FETCH_FRAMES*2];
} spmix_voice;

typedef struct {
 uint32_t output_rate;
 uint16_t master_gain_q8;
 uint64_t frames_rendered;
 uint32_t clipped_samples, starved_reads, input_errors, refill_calls;
 spmix_voice voice[SPMIX_MAX_VOICES];
} spmix_mixer;

/* return 0 on success or -1 invalid arguments; rates in [8000,192000].
 * Output frames always stereo, S16 interleaved; all gain in 0..256 (256=unity).
 */
int spmix_init(spmix_mixer *m, uint32_t output_rate);
int spmix_attach(spmix_mixer *m, unsigned slot, spmix_pull_fn cb, void *ctx,
                 uint32_t input_rate, unsigned channels, unsigned gain_q8, int pan);
void spmix_detach(spmix_mixer *m, unsigned slot);
int spmix_set_gain(spmix_mixer *m, unsigned slot, unsigned target_gain_q8,
                   uint32_t ramp_output_frames);
int spmix_set_pan(spmix_mixer *m, unsigned slot, int pan);
void spmix_set_master_gain(spmix_mixer *m, unsigned gain_q8);
unsigned spmix_active_voices(const spmix_mixer *m);
size_t spmix_render(spmix_mixer *m, int16_t *interleaved_stereo, size_t output_frames);

/* A bounded producer/consumer PCM queue suitable for MP3 push callbacks.
 * Not thread-safe: protect against concurrent IRQ or other threads externally.
 * Buffer is supplied by caller, capacity_frames >=2 and channels 1 or 2.
 */
typedef struct {
 int16_t *pcm;
 unsigned capacity_frames, channels, read_pos, write_pos, count, closed;
 uint32_t overruns;
} spmix_ring;
int spmix_ring_init(spmix_ring *q,int16_t *buffer,unsigned capacity_frames,unsigned channels);
unsigned spmix_ring_write(spmix_ring *q,const int16_t *pcm,unsigned frames);
int spmix_ring_read(void *q,int16_t *dst,unsigned max_frames);
void spmix_ring_close(spmix_ring *q);
void spmix_ring_reset(spmix_ring *q);

/* Optional output sink (e.g. Stage10 DMA audio HAL). Open once before pump.
 * A write return !=0 means a backend failure; no implicit retry or duplication.
 */
typedef struct {
 void *ctx;
 int (*open)(void *,uint32_t,unsigned);
 int (*write)(void *,const int16_t *,size_t);
 void (*close)(void *);
} spmix_sink;
int spmix_sink_open(spmix_sink *sink,spmix_mixer *m);
int spmix_pump(spmix_sink *sink,spmix_mixer *m,int16_t *scratch,size_t scratch_frames);
void spmix_sink_close(spmix_sink *sink);
#ifdef __cplusplus
}
#endif
#endif
