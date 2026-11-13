#include "spmp_stock_bridge.h"
#include <assert.h>
#include <stdio.h>
static int calls=0;
static spmp_stock_result mock_open(void *u,const char *path) {
    (void)u;assert(path[0]=='s');calls++;return SPMP_STOCK_OK;
}
static spmp_stock_result mock_fm(void *u,uint32_t kHz) {
    (void)u;assert(kHz==101700u);calls++;return SPMP_STOCK_OK;
}
int main(void) {
    spmp_stock_bridge b;
    spmp_stock_platform_ops ops={0};
    spmp_stock_bridge_init(&b);
    assert(spmp_stock_bridge_audio(&b,"song.mp3")==SPMP_STOCK_UNAVAILABLE);
    assert(spmp_stock_bridge_fm(&b,101700)==SPMP_STOCK_UNAVAILABLE);
    assert(spmp_stock_bridge_bind(&b,NULL)==SPMP_STOCK_UNAVAILABLE);
    ops.size=sizeof(ops);ops.abi=SPMP_STOCK_BRIDGE_ABI;
    ops.capabilities=SPMP_STOCK_CAP_AUDIO|SPMP_STOCK_CAP_FM;
    ops.audio_open=mock_open;ops.fm_tune_khz=mock_fm;
    assert(spmp_stock_bridge_bind(&b,&ops)==SPMP_STOCK_NOT_VERIFIED);
    ops.verified_on_device=1;
    assert(spmp_stock_bridge_bind(&b,&ops)==SPMP_STOCK_OK);
    assert(spmp_stock_bridge_audio(&b,"song.mp3")==SPMP_STOCK_OK);
    assert(spmp_stock_bridge_fm(&b,101700)==SPMP_STOCK_OK);
    assert(spmp_stock_bridge_fm(&b,70000)==SPMP_STOCK_BAD_ARGUMENT);
    assert(spmp_stock_bridge_image(&b,"s.jpg")==SPMP_STOCK_UNAVAILABLE);
    assert(calls==2);
    puts("bridge: 10 assertions OK (mock only; no device validation)");
    return 0;
}
