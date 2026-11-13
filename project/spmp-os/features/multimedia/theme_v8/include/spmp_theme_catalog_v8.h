#ifndef SPMP_THEME_CATALOG_V8_H
#define SPMP_THEME_CATALOG_V8_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Stable catalog IDs, no libc, no heap. Labels are not in image resources. */
#define SPMP_THEME_V8_COUNT 25
size_t spmp_theme_v8_count(void);
const char *spmp_theme_v8_id(size_t index);
const char *spmp_theme_v8_category(size_t index);
int spmp_theme_v8_find(const char *id);
/* Append slash-separated relative path: themes/<id>/<leaf>. Reject unknown IDs/leaves. */
int spmp_theme_v8_path(size_t index,const char *leaf,char *out,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
