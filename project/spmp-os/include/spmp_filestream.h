#ifndef SPMP_FILESTREAM_H
#define SPMP_FILESTREAM_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Read-only FAT/SD transport supplied by the platform. Handles are opaque. */
typedef struct spmp_file_hal {
  void *context;
  void *(*open_read)(void *context, const char *path);
  int64_t (*size)(void *context, void *handle);
  int64_t (*seek)(void *context, void *handle, int64_t offset, int origin);
  int64_t (*read)(void *context, void *handle, void *dst, int64_t bytes);
  int (*close)(void *context, void *handle);
} spmp_file_hal;
/* Fails if streams still open, allows safe module unload/rebind. */
int spmp_file_bind(const spmp_file_hal *hal);
int spmp_file_unbind(void);
unsigned spmp_file_open_count(void);
#ifdef __cplusplus
}
#endif
#endif
