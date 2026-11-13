#ifndef SPMP_PROBE_OEM_ADAPTER_H
#define SPMP_PROBE_OEM_ADAPTER_H
#include "probe.h"
/* Typed API injection only. No numeric-slot resolver, fixed address, native
 * entry or automatic on-device use. The caller must establish image identity,
 * ABI, resource lifetime and working launch conditions before real bindings.
 */
struct probe_oem_api {
    mmi_init_graph_fn init_graph;
    mmi_set_display_origin_fn set_display_origin;
    mmi_enable_double_buffer_fn enable_double_buffer;
    mmi_get_dimensions_fn dimensions;
    mmi_get_fg_fn get_color;
    mmi_set_fg_fn set_color;
    mmi_fill_rect_fn fill;
    mmi_paint_fn paint;
    mmi_get_keys_fn keys;
};
struct probe_oem_adapter { const struct probe_oem_api *api; };
/* Returned callbacks are tested with injected host functions only. */
const struct probe_ops *probe_oem_operations(void);
#endif
