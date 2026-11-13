/* SPMP-OS host-only shim for PicoDrive Libretro.
   Full libpicofe source is an external submodule absent from supplied zip.
   libretro.c uses PXMAKE for 24-bit RGB -> RGB565 conversion. */
#ifndef SPMP_LIBPICOFE_PLAT_H
#define SPMP_LIBPICOFE_PLAT_H
#define PXMAKE(r,g,b) (((((unsigned)(r)) << 8) & 0xf800u) | (((((unsigned)(g)) << 3) & 0x07e0u)) | (((unsigned)(b)) >> 3))
#endif
