/* ARMv5 freestanding build compatibility declarations.
 * These are ABI declarations, NOT implementations. The SPMP-OS runtime must supply them.
 */
#ifndef SPMP_PICO_ABI_DECL_H
#define SPMP_PICO_ABI_DECL_H
#include <stdint.h>
#ifndef _OFF_T_DECLARED
#define _OFF_T_DECLARED 1
typedef int64_t off_t;
#endif
extern char *strdup(const char *source);
#endif
