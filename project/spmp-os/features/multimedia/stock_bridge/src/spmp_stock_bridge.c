#include "spmp_stock_bridge.h"
#include <stddef.h>

void spmp_stock_bridge_init(spmp_stock_bridge *b) {
    if (!b) return;
    b->platform = NULL;
    b->capabilities = 0;
}

spmp_stock_result spmp_stock_bridge_bind(spmp_stock_bridge *b,const spmp_stock_platform_ops *ops) {
    if (!b) return SPMP_STOCK_BAD_ARGUMENT;
    spmp_stock_bridge_init(b);
    if (!ops) return SPMP_STOCK_UNAVAILABLE;
    if (ops->size != sizeof(*ops) || ops->abi != SPMP_STOCK_BRIDGE_ABI)
        return SPMP_STOCK_INCOMPATIBLE_ABI;
    if (!ops->verified_on_device) return SPMP_STOCK_NOT_VERIFIED;
    if ((ops->capabilities & ~SPMP_STOCK_CAPS_ALL) != 0) return SPMP_STOCK_BAD_ARGUMENT;
    if ((ops->capabilities & SPMP_STOCK_CAP_AUDIO) && !ops->audio_open) return SPMP_STOCK_BAD_ARGUMENT;
    if ((ops->capabilities & SPMP_STOCK_CAP_VIDEO) && !ops->video_open) return SPMP_STOCK_BAD_ARGUMENT;
    if ((ops->capabilities & SPMP_STOCK_CAP_IMAGE) && !ops->image_open) return SPMP_STOCK_BAD_ARGUMENT;
    if ((ops->capabilities & SPMP_STOCK_CAP_FM) && !ops->fm_tune_khz) return SPMP_STOCK_BAD_ARGUMENT;
    b->platform = ops;
    b->capabilities = ops->capabilities;
    return SPMP_STOCK_OK;
}

spmp_stock_result spmp_stock_bridge_audio(spmp_stock_bridge *b,const char *path) {
    if (!b || !path || !*path) return SPMP_STOCK_BAD_ARGUMENT;
    if (!b->platform || !(b->capabilities & SPMP_STOCK_CAP_AUDIO)) return SPMP_STOCK_UNAVAILABLE;
    return b->platform->audio_open(b->platform->user,path);
}
spmp_stock_result spmp_stock_bridge_video(spmp_stock_bridge *b,const char *path) {
    if (!b || !path || !*path) return SPMP_STOCK_BAD_ARGUMENT;
    if (!b->platform || !(b->capabilities & SPMP_STOCK_CAP_VIDEO)) return SPMP_STOCK_UNAVAILABLE;
    return b->platform->video_open(b->platform->user,path);
}
spmp_stock_result spmp_stock_bridge_image(spmp_stock_bridge *b,const char *path) {
    if (!b || !path || !*path) return SPMP_STOCK_BAD_ARGUMENT;
    if (!b->platform || !(b->capabilities & SPMP_STOCK_CAP_IMAGE)) return SPMP_STOCK_UNAVAILABLE;
    return b->platform->image_open(b->platform->user,path);
}
spmp_stock_result spmp_stock_bridge_fm(spmp_stock_bridge *b,uint32_t frequency_khz) {
    if (!b) return SPMP_STOCK_BAD_ARGUMENT;
    if (!b->platform || !(b->capabilities & SPMP_STOCK_CAP_FM)) return SPMP_STOCK_UNAVAILABLE;
    if (frequency_khz < 76000 || frequency_khz > 108000) return SPMP_STOCK_BAD_ARGUMENT;
    return b->platform->fm_tune_khz(b->platform->user,frequency_khz);
}
void spmp_stock_bridge_stop(spmp_stock_bridge *b) {
    if (b && b->platform && b->platform->stop) b->platform->stop(b->platform->user);
}
