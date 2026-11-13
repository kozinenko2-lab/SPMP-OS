
#ifndef _SHARED_H_
#define _SHARED_H_

#include "types.h"
#include "h6280.h"
#include "pce.h"
#include "vdc.h"
#include "vce.h"
#include "render.h"
#include "system.h"
#include "psg.h"
#include "cpu/cpuintrf.h"
#ifdef SPMP_TGEMU_NO_ZIP
/* SPMP-OS loads uncompressed HuCards via bounded HAL read, not zlib */
#else
#include "unzip.h"
#endif
#ifdef SPMP_TGEMU_NO_ZIP
int load_file(char *filename, uint8 *buf, int size);
int save_file(char *filename, uint8 *buf, int size);
#else
#include "fileio.h"
#endif
//#include "osd.h"
#include <string.h>
#include <stdlib.h>

#endif /* _SHARED_H_ */
