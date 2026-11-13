#ifndef SPMP_FAT_BRIDGE_H
#define SPMP_FAT_BRIDGE_H
#include "spmp_fat_reader.h"
/* First member of an integrator context, or pass &context.bridge directly. */
typedef struct { spmp_fat_io io; } spmp_fat_bridge_context;
/* Stage41 spmp_db_ops.read_package compatible callback.
 * Does not touch hardware except via io.read_sector(). */
int spmp_fat_bridge_read_package(void *context,const char *path,
                                 uint8_t *output,size_t capacity,size_t *actual);
#endif
