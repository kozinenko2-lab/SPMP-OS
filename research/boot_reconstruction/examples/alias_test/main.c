#include "startup.h"
#include "alias_test.h"

uint32_t spmp_alias_binding_status;
uint32_t spmp_input_bind(uintptr_t,struct input_services *);

uint32_t spmp_app_main(uintptr_t raw)
{
 struct input_services io;
 spmp_alias_binding_status=spmp_input_bind(raw,&io);
 if(spmp_alias_binding_status)return spmp_alias_binding_status;
 return alias_test_run(&io);
}
