#ifndef SPMP_FAKE_STDLIB_H
#define SPMP_FAKE_STDLIB_H
#include <stddef.h>
/* No heap is linked in this profile. Remaining references fail the symbol audit. */
void *malloc(size_t);
void *calloc(size_t,size_t);
void free(void *);
void abort(void);
#endif
