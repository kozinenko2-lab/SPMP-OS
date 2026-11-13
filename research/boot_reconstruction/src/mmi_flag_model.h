#ifndef SPMP_MMI_FLAG_MODEL_H
#define SPMP_MMI_FLAG_MODEL_H
#include <stdint.h>
/* Numeric modes verified in ARM 0x33CE74; no assumption about external headers. */
#define MMI_FLAG_CLEAR 1u
#define MMI_FLAG_OR 2u
uint32_t mmi_flag_poll_model(uint32_t *bits,uint32_t mask,unsigned mode);
#endif
