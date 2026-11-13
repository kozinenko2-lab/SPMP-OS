#ifndef SPMP_ALLOCA_SHIM_H
#define SPMP_ALLOCA_SHIM_H
#define alloca(sz) __builtin_alloca(sz)
#endif
