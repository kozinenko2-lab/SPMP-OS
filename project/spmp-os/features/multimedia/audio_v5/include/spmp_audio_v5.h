#ifndef SPMP_AUDIO_V5_H
#define SPMP_AUDIO_V5_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { SP_AUD_OK=0, SP_AUD_EOF=1, SP_AUD_BAD=-1, SP_AUD_UNSUPPORTED=-2, SP_AUD_IO=-3, SP_AUD_NO_MEMORY=-4, SP_AUD_NEED_BACKEND=-5 };
enum { SP_FMT_AUTO=0, SP_FMT_WAV=1, SP_FMT_FLAC=2, SP_FMT_OGG_VORBIS=3, SP_FMT_WMA_ASF=4, SP_FMT_AMR_NB=5, SP_FMT_AMR_WB=6 };
typedef struct {
 void *user;
 int (*read)(void *user, void *dst, unsigned bytes); /* bytes read, 0 EOF, negative error */
 int (*seek)(void *user, uint64_t offset); /* 0 success, negative error */
 int64_t (*size)(void *user); /* -1 unknown */
} sp_audio_io;
typedef struct {uint32_t rate,bitrate;uint64_t total_frames;uint16_t channels,bits,format,codec;} sp_audio_info;
typedef struct sp_audio_player sp_audio_player;
/* Optional external codec backend for WMA/AMR. Decode callbacks are platform- or
 * vendor-provided and deliberately not included: a container probe is NOT
 * equivalent to WMA/AMR decoding. All buffers belong to the calling platform. */
typedef struct {
 void *ctx;
 int (*open)(void *ctx,sp_audio_io io,void *workspace,size_t bytes,sp_audio_info *info);
 int (*decode)(void *ctx,int16_t *interleaved,size_t capacity_frames,size_t *frames);
 int (*seek_ms)(void *ctx,uint64_t ms);
 void (*close)(void *ctx);
} sp_audio_external_backend;
int sp_audio_open_with_backend(sp_audio_player *p,sp_audio_io io,int format,
     void *workspace,size_t workspace_bytes,const sp_audio_external_backend *backend);
size_t sp_audio_player_size(void);
/* FLAC requires >=192 KiB workspace; WAV PCM any nonnull workspace;
 * IMA ADPCM workspace >= 2*max_decoded_block_samples bytes;
 * Tremor uses malloc/calloc/realloc/free through the system allocator;
 * host-generated memory measurements are not a device RAM guarantee. */
int sp_audio_open(sp_audio_player *p,sp_audio_io io,int format,void *workspace,size_t workspace_bytes);
int sp_audio_decode(sp_audio_player *p,int16_t *out,size_t capacity_frames,size_t *frames_written);
int sp_audio_seek_ms(sp_audio_player *p,uint64_t millis);
void sp_audio_close(sp_audio_player *p);
sp_audio_info sp_audio_get_info(const sp_audio_player *p);
int sp_audio_detect(const uint8_t *data,size_t size);
/* Platform adapter: send decoded PCM to any SPMP-OS audio sink (DMA/DAC HAL).
 * Caller owns scratch and the sink. No implicit background playback/thread. */
typedef struct {
 void *ctx;
 int (*open)(void *ctx,uint32_t rate,unsigned channels);
 int (*write)(void *ctx,const int16_t *pcm,size_t frames);
 void (*close)(void *ctx);
} sp_audio_sink;
/* Decodes and writes up to scratch_frames. Return 0, 1 EOF, negative error. */
int sp_audio_pump(sp_audio_player *p,sp_audio_sink sink,int16_t *scratch,
                  size_t scratch_frames,size_t *frames_written);
/* Parses metadata for AMR/WMA without claiming an active decoder. */
int sp_audio_probe(sp_audio_io io,int format,sp_audio_info *out);
#ifdef __cplusplus
}
#endif
#endif
