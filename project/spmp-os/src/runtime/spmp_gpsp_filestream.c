/* gpSP read-only filestream adapter for SPMP-OS; no hosted I/O. */
#include "spmp_filestream.h"
#include <stdint.h>
#include <stddef.h>
#include <libretro.h>
#include <streams/file_stream.h>

#define SPMP_FILE_SLOTS 4
struct RFILE { void *handle; unsigned in_use; };
static struct RFILE g_files[SPMP_FILE_SLOTS];
static spmp_file_hal g_hal;
static unsigned g_bound;
static unsigned g_count;

int spmp_file_bind(const spmp_file_hal *hal) {
  if (g_count || !hal || !hal->open_read || !hal->size || !hal->seek || !hal->read || !hal->close)
    return -1;
  g_hal = *hal;
  g_bound = 1;
  return 0;
}
int spmp_file_unbind(void) {
  if (g_count) return -1;
  g_bound = 0;
  g_hal.context = 0;
  g_hal.open_read = 0;
  g_hal.size = 0;
  g_hal.seek = 0;
  g_hal.read = 0;
  g_hal.close = 0;
  return 0;
}
unsigned spmp_file_open_count(void) { return g_count; }

/* Libretro passes its VFS interface here; this port intentionally uses only
 * the verified SPMP-OS HAL. No implicit POSIX or VFS fallback. */
void filestream_vfs_init(const struct retro_vfs_interface_info *info) { (void)info; }

RFILE *filestream_open(const char *path, unsigned mode, unsigned hints) {
  unsigned i;
  void *h;
  (void)hints;
  if (!g_bound || !path || !(mode & RETRO_VFS_FILE_ACCESS_READ) ||
      (mode & RETRO_VFS_FILE_ACCESS_WRITE)) return 0;
  for (i=0;i<SPMP_FILE_SLOTS;i++) if (!g_files[i].in_use) break;
  if (i==SPMP_FILE_SLOTS) return 0;
  h=g_hal.open_read(g_hal.context,path);
  if (!h) return 0;
  g_files[i].handle=h;
  g_files[i].in_use=1;
  ++g_count;
  return &g_files[i];
}
static int valid(RFILE *f) {
  unsigned i;
  if (!f || !g_bound) return 0;
  for (i=0;i<SPMP_FILE_SLOTS;i++) if (f==&g_files[i]) return f->in_use;
  return 0;
}
int64_t filestream_get_size(RFILE *f) {
  return valid(f) ? g_hal.size(g_hal.context,f->handle) : -1;
}
int64_t filestream_seek(RFILE *f,int64_t offset,int origin) {
  if (!valid(f) || origin < 0 || origin > 2) return -1;
  return g_hal.seek(g_hal.context,f->handle,offset,origin);
}
int64_t filestream_read(RFILE *f,void *dst,int64_t len) {
  int64_t n;
  if (!valid(f) || len<0 || (len && !dst)) return -1;
  if (!len) return 0;
  n=g_hal.read(g_hal.context,f->handle,dst,len);
  return (n>=0 && n<=len) ? n : -1;
}
int filestream_close(RFILE *f) {
  int result;
  if (!valid(f)) return -1;
  result=g_hal.close(g_hal.context,f->handle);
  f->handle=0;
  f->in_use=0;
  --g_count;
  return result;
}
