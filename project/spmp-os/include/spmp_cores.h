/* SPMP-OS native core interface v1. Host-proven prototype, NOT an SPMP hardware ABI.
 * This interface is intentionally independent of the historical eCos/NGame ABI.
 */
#ifndef SPMP_CORES_H
#define SPMP_CORES_H
#include <stddef.h>
#include <stdint.h>
#define SPMP_CORE_API_VERSION 5u

typedef enum {
    SPMP_CORE_HOST_TESTED=1, /* Code can run under the host harness */
    SPMP_CORE_SOURCE_AVAILABLE=2, /* User source available; adapter not linked */
    SPMP_CORE_SOURCE_UNVERIFIED=3 /* No confirmed native source adapter */
} spmp_core_state;

typedef struct {
    void *user;
    /* RGB565 frame owned by core, valid until callback returns. */
    void (*present)(void *user,const uint16_t *rgb565,unsigned width,unsigned height);
    uint32_t (*buttons)(void *user);
    /* File reading delegated to platform HAL. No libc in core. Return 0 on
     * success, -1 on failure; reject oversized content explicitly. */
    int (*read_entire)(void *user,const char *path,uint8_t *dst,
                       size_t capacity,size_t *length);
    /* Optional signed 16-bit interleaved stereo PCM sink. */
    void (*audio_pcm)(void *user,const int16_t *samples,size_t frames,unsigned sample_rate);
    /* Optional per-game battery RAM storage. Paths are passed as opaque game
     * identifiers; the platform decides where to store data safely. */
    int (*load_save)(void *user, const char *core_id, const char *game_path,
                     uint8_t *dst, size_t capacity, size_t *loaded);
    int (*store_save)(void *user, const char *core_id, const char *game_path,
                      const uint8_t *src, size_t bytes);
    /* Optional bounded preflight for memory-constrained consoles. The HAL
     * reports the exact on-disk size without reading the file. Cores that
     * support this callback never allocate LR_MAX_ROM for a tiny cartridge.
     * Return -1 for missing files or unknown sizes; 0 for exact size. */
    int (*file_size)(void *user, const char *path, size_t *bytes);
    /* Later revisions: virtual filesystem, monotonic time, config, lifecycle. */
} spmp_core_host;

typedef struct {
    int (*start)(const spmp_core_host *host,const char *content);
    int (*tick)(void); /* 0 continue, +1 exit, -1 error */
    void (*stop)(void);
    size_t (*state_size)(void);  /* optional full emulator savestate */
    int (*serialize)(void *dst,size_t exact_bytes);
    int (*unserialize)(const void *src,size_t bytes);
    /* Optional per-core setting apply, 0 live, 1 deferred, -1 reject. */
    int (*set_option)(unsigned id,int value);
} spmp_core_ops;

typedef struct {
    const char *id;
    const char *title;
    const char *source_archive;
    const char *content_extensions; /* semicolon-separated or '-' */
    spmp_core_state state;
    const spmp_core_ops *ops; /* NULL until integrated */
} spmp_core_desc;

typedef enum {
    SPMP_MATCH_NONE=0,
    SPMP_MATCH_UNIQUE=1,
    SPMP_MATCH_AMBIGUOUS=2
} spmp_match_result;

size_t spmp_core_count(void);
const spmp_core_desc *spmp_core_at(size_t index);
const spmp_core_desc *spmp_core_find(const char *id);
/* Each candidate is returned as pointer to constant static metadata.
 * More than one candidate means the menu must ask the user or inspect contents.
 * A unique match does NOT imply the core is runnable.
 */
/* Signature-aware resolution: header is a bounded prefix (up to 64 KiB). A
 * known unambiguous format overrides filename extension. Unknown/weak file
 * signatures keep the extension candidates, including ambiguity. */
spmp_match_result spmp_core_resolve_data(const char *filename,
    const uint8_t *header,size_t header_size,
    const spmp_core_desc **out,size_t out_capacity,size_t *total);
spmp_match_result spmp_core_resolve(const char *filename,
    const spmp_core_desc **out,size_t out_capacity,size_t *total);
int spmp_core_start(const char *id,const spmp_core_host *host,const char *content);
int spmp_core_tick(void);
void spmp_core_stop(void); /* Discards ROM state; keeps core module resident */
int spmp_core_pause_to_menu(void); /* Keeps both ROM and core in RAM */
int spmp_core_resume(void);
int spmp_core_is_paused(void); /* tick() returns 2 while paused */
int spmp_core_launch(const char *id,const spmp_core_host *host,const char *content);
const char *spmp_core_cached_id(void);
size_t spmp_core_state_size(void); /* 0 when unsupported */
int spmp_core_serialize(void *dst,size_t bytes); /* paused game only */
int spmp_core_unserialize(const void *src,size_t bytes);
int spmp_core_apply_setting(unsigned id,int value); /* paused game only */
void spmp_core_shutdown(void); /* Explicit release, including module code */
#endif
