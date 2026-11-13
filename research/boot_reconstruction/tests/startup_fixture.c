/* VM fixture only. Replaces application.c at link time. No OEM pointer binding.
 * Python injects typed callbacks into this object's .data before VM execution.
 */
#include "startup.h"
#include "oem_adapter.h"
struct startup_vm_configuration {
    uint32_t cookie,expected_raw_api;
    struct probe_oem_api api;
    struct probe_config config;
};
volatile struct startup_vm_configuration spmp_vm_configuration={.cookie=0x564d3031u};
struct probe_state spmp_vm_probe_state;
uint32_t spmp_vm_raw_api,spmp_vm_result;
uint32_t spmp_app_main(uintptr_t raw_api_address)
{
    spmp_vm_raw_api=(uint32_t)raw_api_address;
    if(spmp_vm_configuration.cookie!=0x564d3031u ||
       raw_api_address!=spmp_vm_configuration.expected_raw_api)return SPMP_STARTUP_UNBOUND;
    /* Copy volatile injected data to normal typed objects; both live until return. */
    struct probe_oem_api api=spmp_vm_configuration.api;
    struct probe_config config=spmp_vm_configuration.config;
    struct probe_oem_adapter adapter={&api};
    enum probe_result result=probe_start(&spmp_vm_probe_state,probe_oem_operations(),&adapter,&config);
    while(result==PROBE_RUNNING)result=probe_step(&spmp_vm_probe_state);
    spmp_vm_result=(uint32_t)result;
    return (uint32_t)result;
}
_Static_assert(sizeof(struct startup_vm_configuration)==68,"ARM32 VM injection layout");
