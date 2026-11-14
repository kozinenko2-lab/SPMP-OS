#include "startup.h"
#include "input_test.h"
struct input_state spmp_input_state;
uint32_t spmp_input_binding_status;
/* Initialized writable config allows VM scenarios without changing production entry ABI. */
struct input_config spmp_input_config={60000,3000,4000,100};
uint32_t spmp_input_bind(uintptr_t,struct input_services *);
uint32_t spmp_app_main(uintptr_t raw)
{
 struct input_services io;
 spmp_input_binding_status=spmp_input_bind(raw,&io);
 if(spmp_input_binding_status)return spmp_input_binding_status;
 return input_test_run(&spmp_input_state,&io,&spmp_input_config);
}
