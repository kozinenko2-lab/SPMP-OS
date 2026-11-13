#ifndef SPMP_GBA_CART_H
#define SPMP_GBA_CART_H
#include <stdint.h>
#include <stddef.h>
/* GBA header is exactly 0xC0 bytes. 0=valid, negative=corrupt/invalid. */
int spmp_gba_validate_header(const uint8_t *header,size_t length);
#endif
