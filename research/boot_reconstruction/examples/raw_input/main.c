#include "startup.h"
#include "raw_input.h"
struct raw_state spmp_raw_state;
uint32_t spmp_raw_binding_status;
struct raw_config spmp_raw_config={80000,4000,100};
uint32_t spmp_input_bind(uintptr_t,struct input_services *);
uint32_t spmp_app_main(uintptr_t raw)
{
 struct input_services io;
 spmp_raw_binding_status=spmp_input_bind(raw,&io);
 if(spmp_raw_binding_status)return spmp_raw_binding_status;
 return raw_input_run(&spmp_raw_state,&io,spmp_raw_snapshot,&spmp_raw_config);
}
