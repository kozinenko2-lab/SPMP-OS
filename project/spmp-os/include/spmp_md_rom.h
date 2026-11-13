#ifndef SPMP_MD_ROM_H
#define SPMP_MD_ROM_H
#include <stddef.h>
#include <stdint.h>
/* Returns 1 for raw Mega Drive, 2 for copier SMD, 0 unknown. */
/* Recognize an SMD copier header from the first 16.5 KiB only, so ROM
 * classification does not need the full file size or a whole ROM read. */
int spmp_md_smd_prefix(const uint8_t *prefix,size_t available);
int spmp_md_format(const uint8_t *rom,size_t bytes);
/* Converts SMD to normal ROM IN PLACE, using caller-provided 16 KiB workspace.
 * Raw ROMs are not changed. On error leaves original buffer unchanged.
 * No allocation or OS dependency; suitable for ARMv5 runtime and HAL.
 */
int spmp_md_canonicalize(uint8_t *rom,size_t *bytes,uint8_t *work,size_t work_cap);
#endif
