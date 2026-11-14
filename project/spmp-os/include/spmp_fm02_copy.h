/* SPDX-License-Identifier: MIT */
#ifndef SPMP_FM02_COPY_H
#define SPMP_FM02_COPY_H
#include "spmp_file_copy.h"
#include "spmp_fm02.h"
struct spmp_fm02_copy { struct spmp_file_copy task; uint32_t sequence; };
/* fm02_backend_action adapter. COPY of regular files only; other actions return
 * -ENOSYS without IO. Host/ARM tested, no live FAT binding yet. Synchronous FM02
 * gets one progress/cancel opportunity per engine step. */
int spmp_fm02_copy_action(void *,const fm02_request *,fm02_progress_hook,void *,fm02_progress *);
#endif
