#ifndef SPMP_NATIVE_BINDING_H
#define SPMP_NATIVE_BINDING_H
#include "oem_adapter.h"
enum native_binding_status {
    NATIVE_BOUND=0,NATIVE_BAD_API=0x901,NATIVE_BAD_SLOT=0x902,
    NATIVE_BAD_CODE=0x903,NATIVE_BAD_ALLOCATION=0x904,NATIVE_BAD_STACK=0x905,
    NATIVE_BAD_FRAMEBUFFERS=0x906
};
uint32_t spmp_native_preflight(uintptr_t raw_api);
uint32_t spmp_native_bind(uintptr_t raw_api,struct probe_oem_api *out);
#endif
