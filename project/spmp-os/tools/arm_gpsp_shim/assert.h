#ifndef SPMP_GPSP_ASSERT_H
#define SPMP_GPSP_ASSERT_H
#ifdef NDEBUG
#define assert(x) ((void)0)
#else
void spmp_gpsp_assert_fail(const char *, const char *, int);
#define assert(x) ((x) ? (void)0 : spmp_gpsp_assert_fail(#x, __FILE__, __LINE__))
#endif
#endif
