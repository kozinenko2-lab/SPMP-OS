#pragma once
struct timeval { long tv_sec; long tv_usec; };
#ifdef __cplusplus
extern "C" {
#endif
int gettimeofday(struct timeval *, void *);
#ifdef __cplusplus
}
#endif
