/* SPMP-OS Stage34: read-only diagnostics routed exclusively through bound,
 * platform-verified callbacks. No guessed eCos function pointers or MMIO. */
#ifndef SPMP_DIAG_HAL_H
#define SPMP_DIAG_HAL_H
#include <stdint.h>
#define SPMP_DIAG_KEYS  1u
#define SPMP_DIAG_TIME  2u
#define SPMP_DIAG_SD    4u
#define SPMP_DIAG_LCD   8u
#define SPMP_DIAG_AUDIO 16u

typedef struct {
    void *context;
    int (*read_keys)(void *context,uint32_t *keys);
    int (*read_monotonic_ms)(void *context,uint32_t *ms);
    int (*read_sd_presence)(void *context,int *present);
    int (*read_lcd_info)(void *context,uint16_t *w,uint16_t *h);
    int (*read_audio_info)(void *context,uint32_t *rate,uint8_t *channels);
    /* Established by separately audited platform integration, not this API. */
    int (*confirm_readonly_api)(void *context);
} spmp_diag_hal;
typedef struct {
    uint32_t available, failed, keys, milliseconds, sample_rate_hz;
    uint16_t lcd_width,lcd_height;
    uint8_t audio_channels,sd_present;
} spmp_diag_snapshot;
/* Takes one snapshot. No SD mount/unmount, writes, screen or audio calls. */
int spmp_diag_readonly_snapshot(const spmp_diag_hal *h,spmp_diag_snapshot *dst);
#endif
