#include "input_test.h"
#include "native_binding.h"
_Static_assert(sizeof(struct input_services)==64,"ARM32 input services layout");
struct input_fingerprint { uint32_t address,crc32; };
#ifdef SPMP_BINDING_SYNTHETIC
#include "input_profile_synthetic.h"
#else
#include "input_profile.h"
#endif
uint32_t spmp_input_preflight_extra(void)
{
 for(unsigned i=0;i<7;i++){
  const volatile uint8_t *p=(const volatile uint8_t *)(uintptr_t)input_prefixes[i].address;
  uint32_t crc=0xffffffffu;
  for(unsigned n=0;n<32;n++){
   crc^=p[n];for(unsigned bit=0;bit<8;bit++)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));
  }
  if((~crc)!=input_prefixes[i].crc32)return 0x907;
 }
 return 0;
}
static uint32_t error_number(void)
{
 typedef volatile uint32_t *(*errno_location_fn)(void);
 volatile uint32_t *p=((errno_location_fn)(uintptr_t)0x299a14)();
 return p?*p:0xffffffffu;
}
uint32_t spmp_input_bind(uintptr_t raw,struct input_services *out)
{
 uint32_t status=spmp_native_bind(raw,&out->graphics);if(status)return status;
 out->time=(input_time_fn)(uintptr_t)0x2d3254;
 out->delay=(input_delay_fn)(uintptr_t)0x339be8;
 out->open=(input_open_fn)(uintptr_t)0x2a5ed4;
 out->write=(input_write_fn)(uintptr_t)0x2eb610;
 out->seek=(input_seek_fn)(uintptr_t)0x2eb69c;
 out->close=(input_close_fn)(uintptr_t)0x2eb640;
 out->error_number=error_number;
 return 0;
}
