#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
unsigned spmp_udivmod32(unsigned,unsigned,unsigned *);
static uint32_t seed=0x14af73d2;
static uint32_t next(void) { seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed; }
int main(void) {
  unsigned checks=0;
  for(unsigned i=0;i<200000;i++) {
    unsigned n=next(),d=next()|1u,r=~0u;
    unsigned q=spmp_udivmod32(n,d,&r);
    assert(q==n/d && r==n%d);
    checks+=2;
  }
  const struct {const char *s;int base;long expected;size_t consumed;} t[]={
    {"0",0,0,1},{"0x10",0,16,4},{"0Xff!",16,255,4},
    {"   -12345end",10,-12345,9},{"+0b11",0,0,2},
    {"0777",0,511,4},{"2A",16,42,2}, {"zz",36,1295,2},
    {"  +",10,0,0},{"xyz",10,0,0},{"0129",0,10,3},
    {"2147483647",10,2147483647L,10},{"-2147483648",10,-2147483648L,11}
  };
  for(size_t i=0;i<sizeof(t)/sizeof(t[0]);i++) {
    char *end=0;long actual=strtol(t[i].s,&end,t[i].base);
    assert(actual==t[i].expected);
    assert((size_t)(end-t[i].s)==t[i].consumed);
    checks+=2;
  }
  assert(atoi(" -91 !")==-91);checks++;
  printf("stage30 numeric checks: %u passed\n",checks);
  return 0;
}
