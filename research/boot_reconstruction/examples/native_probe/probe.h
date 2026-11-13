#ifndef SPMP_RESEARCH_PROBE_H
#define SPMP_RESEARCH_PROBE_H
#include "mmi_native_abi.h"
/* Portable probe core: HOST_TESTED / ARM_OBJECT_ONLY. No OEM pointer binding,
 * target entry, linker image or NGame package. Caller owns ops/config/state.
 */
struct probe_rect { int16_t x,y; uint16_t width,height; };
struct probe_ops {
    uint32_t (*begin)(void *);
    uint32_t (*dimensions)(void *,uint16_t *,uint16_t *);
    uint32_t (*get_color)(void *,struct mmi_rgb888 *);
    uint32_t (*set_color)(void *,const struct mmi_rgb888 *);
    uint32_t (*fill)(void *,const struct probe_rect *);
    uint32_t (*present)(void *);
    uint32_t (*keys)(void *,struct mmi_key_output32 *);
    void (*finish)(void *);
};
struct probe_config {
    uint32_t max_steps,up,down,left,right,quit;
};
enum probe_result { PROBE_RUNNING=0,PROBE_RETURN=1,PROBE_ERROR=2 };
struct probe_state {
    const struct probe_ops *ops;
    const struct probe_config *config;
    void *user;
    struct mmi_rgb888 original_color;
    uint32_t steps,last_keys;
    int16_t x,y;
    uint8_t active,color_saved,finished;
    enum probe_result result;
};
enum probe_result probe_start(struct probe_state *,const struct probe_ops *,void *,const struct probe_config *);
/* One bounded poll/render step; caller supplies scheduling between steps.
 * PROBE_RETURN means return normally from the future application's caller.
 * Never calls native gameExit, changes SP or initializes audio/resources.
 */
enum probe_result probe_step(struct probe_state *);
#endif
