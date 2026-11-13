#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
extern uint64_t spmp_udivmod64(uint64_t, uint64_t, uint64_t *);
static uint64_t seed = UINT64_C(0x9e3779b97f4a7c15);
static uint64_t rnd(void) { seed ^= seed >> 12; seed ^= seed << 25; seed ^= seed >> 27; return seed * UINT64_C(2685821657736338717); }
static unsigned long checks;
static void check(uint64_t n, uint64_t d) {
    uint64_t rem=0, q=spmp_udivmod64(n,d,&rem);
    uint64_t expq=d?n/d:0, expr=d?n%d:n;
    ++checks;
    if(q!=expq || rem!=expr) {
        fprintf(stderr,"FAILED n=%"PRIu64" d=%"PRIu64" got q=%"PRIu64" r=%"PRIu64" expected q=%"PRIu64" r=%"PRIu64"\n",n,d,q,rem,expq,expr);
        exit(1);
    }
}
int main(void) {
    const uint64_t a[]={0,1,2,3,UINT64_MAX,UINT64_MAX-1,UINT64_C(0x8000000000000000),UINT64_C(0x7fffffffffffffff),UINT64_C(0x100000000),UINT64_C(0xffffffff)};
    for(unsigned i=0;i<sizeof(a)/sizeof(a[0]);i++)for(unsigned j=0;j<sizeof(a)/sizeof(a[0]);j++) check(a[i],a[j]);
    for(unsigned i=0;i<200000;i++) {
        uint64_t n=rnd(),d=rnd();
        if((i&3)==0)d=(uint32_t)d;
        if((i&7)==0)d=n;
        if((i&15)==0)d=1;
        if((i&31)==0)d=0;
        check(n,d);
    }
    printf("stage31 64-bit division: %lu checks passed\n",checks);
    return 0;
}
