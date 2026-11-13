#include "startup.h"
#include "native_binding.h"
struct native_probe_record { uint32_t binding_status,result,steps,last_keys; };
struct native_probe_record spmp_native_record;
struct probe_state spmp_native_probe;
static volatile uint32_t initialized_cookie=0x53504d50u;
uint32_t spmp_app_main(uintptr_t raw_api)
{
    struct probe_oem_api api;
    (void)initialized_cookie;
    uint32_t status=spmp_native_bind(raw_api,&api);
    spmp_native_record.binding_status=status;
    if(status)return status;
    struct probe_oem_adapter adapter={&api};
    const struct probe_config config={1,1,2,4,8,0x80000};
    enum probe_result result=probe_start(&spmp_native_probe,probe_oem_operations(),&adapter,&config);
    while(result==PROBE_RUNNING)result=probe_step(&spmp_native_probe);
    spmp_native_record.result=(uint32_t)result;
    spmp_native_record.steps=spmp_native_probe.steps;
    spmp_native_record.last_keys=spmp_native_probe.last_keys;
    return (uint32_t)result;
}
