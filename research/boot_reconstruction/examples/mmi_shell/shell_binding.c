#include "spmp_mmi_input.h"
#include <stdint.h>
struct shell_fingerprint {uint32_t address,literal,value,crc32;};
#ifdef SPMP_BINDING_SYNTHETIC
#include "shell_profile_synthetic.h"
#else
#include "shell_profile.h"
#endif
uint32_t spmp_shell_preflight(void)
{
 for(unsigned i=0;i<2;i++){
  const volatile uint8_t *p=(const volatile uint8_t *)(uintptr_t)shell_prefixes[i].address;
  uint32_t crc=0xffffffffu;
  for(unsigned n=0;n<16;n++){
   crc^=p[n];for(unsigned b=0;b<8;b++)crc=(crc>>1)^(0xedb88320u&(0u-(crc&1u)));
  }
  if((~crc)!=shell_prefixes[i].crc32||*(const volatile uint32_t *)(uintptr_t)shell_prefixes[i].literal!=shell_prefixes[i].value)return 0x909;
 }
#ifdef SPMP_FILES_BUILD
 extern uint32_t spmp_files_preflight(void);
 return spmp_files_preflight();
#else
 return 0;
#endif
}
void spmp_shell_snapshot(void *user,struct spmp_mmi_snapshot *s)
{
 (void)user;
 s->state=*(const volatile uint32_t *)(uintptr_t)0x556144;
 s->event=*(const volatile uint32_t *)(uintptr_t)0x556148;
 s->input_gate=*(const volatile uint32_t *)(uintptr_t)0x556aa0;
 s->gui_lock=*(const volatile uint32_t *)(uintptr_t)0x561078;
}
uint32_t spmp_shell_collect(void *user)
{
 (void)user;typedef uint32_t (*collector_fn)(void);
 return ((collector_fn)(uintptr_t)0x2d2e6c)();
}
