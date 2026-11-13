#ifndef SPMP_MIN_TIME_H
#define SPMP_MIN_TIME_H
struct timeval { long tv_sec; long tv_usec; };
int gettimeofday(struct timeval*, void*);
#endif
