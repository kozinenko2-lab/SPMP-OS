#include "oem_adapter.h"
static uint32_t begin(void *user)
{
    struct probe_oem_adapter *a=user;
    if(!a||!a->api)return 1;
    const struct probe_oem_api *p=a->api;
    if(!p->init_graph||!p->set_display_origin||!p->enable_double_buffer||!p->dimensions||
       !p->get_color||!p->set_color||!p->fill||!p->paint||!p->keys)return 1;
    uint32_t status=p->init_graph();if(status)return status;
    const struct mmi_display_origin16 origin={0,0};
    status=p->set_display_origin(&origin);if(status)return status;
    return p->enable_double_buffer(1);
}
static uint32_t dimensions(void *u,uint16_t *w,uint16_t *h){return ((struct probe_oem_adapter *)u)->api->dimensions(w,h);}
static uint32_t get_color(void *u,struct mmi_rgb888 *c){return ((struct probe_oem_adapter *)u)->api->get_color(c);}
static uint32_t set_color(void *u,const struct mmi_rgb888 *c){return ((struct probe_oem_adapter *)u)->api->set_color(c);}
static uint32_t fill(void *u,const struct probe_rect *r)
{
    const struct mmi_rect_s16 native={r->x,r->y,r->width,r->height};
    return ((struct probe_oem_adapter *)u)->api->fill(&native);
}
static uint32_t present(void *u){return ((struct probe_oem_adapter *)u)->api->paint();}
static uint32_t keys(void *u,struct mmi_key_output32 *out){return ((struct probe_oem_adapter *)u)->api->keys(out);}
/* Ordinary caller return is requested by the core. Do not call gameExit here.
 * This function does not certify OEM worker deletion or restore the GUI state.
 */
static void finish(void *u){(void)u;}
const struct probe_ops *probe_oem_operations(void)
{
    static const struct probe_ops ops={begin,dimensions,get_color,set_color,fill,present,keys,finish};
    return &ops;
}
