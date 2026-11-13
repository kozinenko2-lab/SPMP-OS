#ifndef SPMP_MEDIA_H
#define SPMP_MEDIA_H
#include <stddef.h>
#include <stdint.h>
typedef enum {
 SPMP_FMT_UNKNOWN, SPMP_FMT_MP3, SPMP_FMT_MP4, SPMP_FMT_AVI,
 SPMP_FMT_JPEG, SPMP_FMT_PNG, SPMP_FMT_GIF, SPMP_FMT_WAV,
 SPMP_FMT_NGAME, SPMP_FMT_ARM_ELF
} spmp_format;
spmp_format spmp_probe_header(const uint8_t *bytes,size_t length);
const char *spmp_format_name(spmp_format format);
#endif
