#include "raw_input.h"
struct raw_fingerprint {uint32_t address,bytes,crc32;};
#ifdef SPMP_BINDING_SYNTHETIC
#include "raw_profile_synthetic.h"
#else
#include "raw_profile.h"
#endif
uint32_t spmp_raw_preflight(void)
{
 for(unsigned i=0;i<3;i++){
  const volatile uint8_t *p=(const volatile uint8_t *)(uintptr_t)raw_prefixes[i].address;
  uint32_t crc=0xffffffffu;
  for(unsigned n=0;n<raw_prefixes[i].bytes;n++){
   crc^=p[n];for(unsigned bit=0;bit<8;bit++)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));
  }
  if((~crc)!=raw_prefixes[i].crc32)return 0x908;
 }
 if(*(const volatile uint32_t *)(uintptr_t)0x2d2e4c!=0x556144u||
    *(const volatile uint32_t *)(uintptr_t)0x2d2e60!=0x556148u||
    *(const volatile uint32_t *)(uintptr_t)0x2d2f4c!=0x556148u||
    *(const volatile uint32_t *)(uintptr_t)0x2d2f50!=0x556144u)return 0x908;
#ifdef SPMP_SHELL_BUILD
 extern uint32_t spmp_shell_preflight(void);
 return spmp_shell_preflight();
#else
 return 0;
#endif
}
void spmp_raw_snapshot(struct raw_snapshot *out)
{
 /* Proven MMI RAM globals. No collector call, GPIO/MMIO access or RAM write. */
 out->state=*(const volatile uint32_t *)(uintptr_t)0x556144;
 out->event=*(const volatile uint32_t *)(uintptr_t)0x556148;
}
