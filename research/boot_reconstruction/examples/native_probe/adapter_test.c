#include "oem_adapter.h"
#include <assert.h>
#include <stdio.h>
static unsigned init_calls,origin_calls,double_calls,paints,polls,fills;
static struct mmi_rgb888 color={16,68,160};
static uint32_t init_graph(void){init_calls++;return 0;}
static uint32_t origin(const struct mmi_display_origin16 *p){origin_calls++;assert(p->x==0&&p->y==0);return 0;}
static uint32_t double_buffer(int32_t enabled){double_calls++;assert(enabled==1);return 0;}
static uint32_t dimensions(uint16_t *w,uint16_t *h){*w=320;*h=240;return 0;}
static uint32_t get_color(struct mmi_rgb888 *p){p->r=color.r;p->g=color.g;p->b=color.b;return 0;}
static uint32_t set_color(const struct mmi_rgb888 *p){color.r=p->r;color.g=p->g;color.b=p->b;return 0;}
static uint32_t fill(const struct mmi_rect_s16 *p){fills++;assert(p->x>=0&&p->y>=0&&p->width&&p->height);return 0;}
static uint32_t paint(void){paints++;return 0;}
static uint32_t keys(struct mmi_key_output32 *p){p->keys=polls++?0x20000:8;return 1;}
int main(void)
{
    const struct probe_oem_api api={init_graph,origin,double_buffer,dimensions,get_color,set_color,fill,paint,keys};
    struct probe_oem_adapter adapter={&api};const struct probe_config config={8,1,2,4,8,0x20000};
    struct probe_state s;const struct probe_ops *ops=probe_oem_operations();
    assert(probe_start(&s,ops,&adapter,&config)==PROBE_RUNNING);
    assert(probe_step(&s)==PROBE_RUNNING&&probe_step(&s)==PROBE_RETURN);
    assert(init_calls==1&&origin_calls==1&&double_calls==1&&paints==1&&polls==2&&fills==4);
    assert(color.r==16&&color.g==68&&color.b==160);
    assert(probe_step(&s)==PROBE_RETURN&&polls==2);
    struct probe_oem_api incomplete=api;incomplete.paint=0;adapter.api=&incomplete;
    assert(probe_start(&s,ops,&adapter,&config)==PROBE_ERROR&&init_calls==1);
    puts("PASS: typed OEM-signature adapter with host injections; init/origin/double-buffer, draw/input and ordinary return request");
    return 0;
}
