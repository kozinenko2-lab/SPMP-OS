/* SPMP-OS: freestanding numeric parsing and ARM unsigned integer division.
 * Deliberately excludes floating point, RTC and formatted I/O.
 */
#include <stdint.h>
#include <limits.h>

static int whitespace(unsigned char c) {
  return c==' ' || (c>='\t' && c<='\r');
}
static int digitval(unsigned char c) {
  if (c>='0' && c<='9') return c-'0';
  if (c>='a' && c<='z') return c-'a'+10;
  if (c>='A' && c<='Z') return c-'A'+10;
  return -1;
}
long strtol(const char *text, char **endptr, int base) {
  const char *p=text;
  while (whitespace((unsigned char)*p)) ++p;
  int neg=0;
  if (*p=='-' || *p=='+') { neg=(*p=='-'); ++p; }
  if (base!=0 && (base<2 || base>36)) {
    if (endptr) *endptr=(char *)text;
    return 0;
  }
  if ((base==0 || base==16) && p[0]=='0' && (p[1]=='x'||p[1]=='X') && digitval((unsigned char)p[2])>=0 && digitval((unsigned char)p[2])<16) {
    p+=2; base=16;
  } else if (base==0) base=(*p=='0')?8:10;
  const char *first=p;
  unsigned long limit=neg ? (unsigned long)LONG_MAX+1UL : (unsigned long)LONG_MAX;
  unsigned long value=0;
  int overflow=0;
  for (;;) {
    int d=digitval((unsigned char)*p);
    if (d<0 || d>=base) break;
    if (!overflow) {
      if (value>(limit-(unsigned long)d)/(unsigned)base) { value=limit; overflow=1; }
      else value=value*(unsigned)base+(unsigned)d;
    }
    ++p;
  }
  if (endptr) *endptr=(char *)(p==first ? text : p);
  if (overflow) return neg?LONG_MIN:LONG_MAX;
  if (neg && value==(unsigned long)LONG_MAX+1UL) return LONG_MIN;
  return neg?-(long)value:(long)value;
}
int atoi(const char *text) {
  return (int)strtol(text, 0, 10);
}
/* Binary long division: no division/modulo op in implementation. */
unsigned spmp_udivmod32(unsigned n, unsigned d, unsigned *rem) {
  if (!d) { if (rem) *rem=n; return 0; } /* caller must guard zero; never divide by zero */
  unsigned q=0, r=0;
  for (unsigned i=0;i<32;i++) {
    unsigned carry=r>>31;
    r=(r<<1)|(n>>31);
    n<<=1;
    q<<=1;
    if (carry || r>=d) { r-=d; q|=1; }
  }
  if (rem) *rem=r;
  return q;
}
#if defined(__arm__)
unsigned __aeabi_uidiv(unsigned n,unsigned d) { return spmp_udivmod32(n,d,0); }
#endif
