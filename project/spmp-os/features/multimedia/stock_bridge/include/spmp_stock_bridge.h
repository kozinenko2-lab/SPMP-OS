#ifndef SPMP_STOCK_BRIDGE_H
#define SPMP_STOCK_BRIDGE_H
/* Independent SPMP-OS multimedia provider boundary. This is NOT a codec loader. */
#include <stdint.h>
#define SPMP_STOCK_BRIDGE_ABI 1u
#define SPMP_STOCK_CAP_AUDIO (1u << 0)
#define SPMP_STOCK_CAP_VIDEO (1u << 1)
#define SPMP_STOCK_CAP_IMAGE (1u << 2)
#define SPMP_STOCK_CAP_FM    (1u << 3)
#define SPMP_STOCK_CAPS_ALL (SPMP_STOCK_CAP_AUDIO|SPMP_STOCK_CAP_VIDEO|SPMP_STOCK_CAP_IMAGE|SPMP_STOCK_CAP_FM)

typedef enum spmp_stock_result {
    SPMP_STOCK_OK = 0,
    SPMP_STOCK_UNAVAILABLE = -1,
    SPMP_STOCK_BAD_ARGUMENT = -2,
    SPMP_STOCK_INCOMPATIBLE_ABI = -3,
    SPMP_STOCK_NOT_VERIFIED = -4
} spmp_stock_result;

typedef struct spmp_stock_platform_ops {
    uint32_t size;
    uint32_t abi;
    uint32_t verified_on_device; /* set by integration layer AFTER real board validation */
    uint32_t capabilities;
    void *user;
    spmp_stock_result (*audio_open)(void *user,const char *path);
    spmp_stock_result (*video_open)(void *user,const char *path);
    spmp_stock_result (*image_open)(void *user,const char *path);
    spmp_stock_result (*fm_tune_khz)(void *user,uint32_t freq_khz);
    void (*stop)(void *user);
} spmp_stock_platform_ops;

typedef struct spmp_stock_bridge {
    const spmp_stock_platform_ops *platform;
    uint32_t capabilities;
} spmp_stock_bridge;

void spmp_stock_bridge_init(spmp_stock_bridge *b);
spmp_stock_result spmp_stock_bridge_bind(spmp_stock_bridge *b,const spmp_stock_platform_ops *ops);
spmp_stock_result spmp_stock_bridge_audio(spmp_stock_bridge *b,const char *path);
spmp_stock_result spmp_stock_bridge_video(spmp_stock_bridge *b,const char *path);
spmp_stock_result spmp_stock_bridge_image(spmp_stock_bridge *b,const char *path);
spmp_stock_result spmp_stock_bridge_fm(spmp_stock_bridge *b,uint32_t frequency_khz);
void spmp_stock_bridge_stop(spmp_stock_bridge *b);
#endif
