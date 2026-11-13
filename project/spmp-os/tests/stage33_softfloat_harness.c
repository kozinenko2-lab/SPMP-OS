/* Host oracle for Stage33 bit-accurate IEEE-754 operations.
 * Build with -fno-fast-math, default FE_TONEAREST, SSE scalar hardware.
 */
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
extern float __aeabi_fadd(float,float);
extern float __aeabi_fsub(float,float);
extern float __aeabi_fmul(float,float);
extern float __aeabi_fdiv(float,float);
extern int __aeabi_fcmpge(float,float);
extern double __aeabi_dmul(double,double);
extern float __aeabi_ui2f(unsigned);
extern double __aeabi_ui2d(unsigned);
extern unsigned __aeabi_f2uiz(float);
extern long long __aeabi_d2lz(double);
static uint64_t rng=UINT64_C(0x9e3779b97f4a7c15);
static uint64_t next64(void){uint64_t x=rng; x^=x>>12; x^=x<<25; x^=x>>27; rng=x;return x*UINT64_C(0x2545f4914f6cdd1d);}
static float ff(uint32_t b){float f;memcpy(&f,&b,4);return f;}
static double dd(uint64_t b){double d;memcpy(&d,&b,8);return d;}
static uint32_t fb(float f){uint32_t b;memcpy(&b,&f,4);return b;}
static uint64_t db(double f){uint64_t b;memcpy(&b,&f,8);return b;}
static int fmatch(float got,float expected){return (isnan(got)&&isnan(expected))|| fb(got)==fb(expected);}
static int dmatch(double got,double expected){return (isnan(got)&&isnan(expected))|| db(got)==db(expected);}
static int fail=0;
static void cf(const char *op, uint32_t aa,uint32_t bb,float got,float want){
  if(!fmatch(got,want)){if(fail<16)fprintf(stderr,"%s %08x %08x => %08x expected %08x\n",op,aa,bb,fb(got),fb(want));++fail;}
}
static void cd(const char *op, uint64_t aa,uint64_t bb,double got,double want){
  if(!dmatch(got,want)){if(fail<16)fprintf(stderr,"%s %016llx %016llx => %016llx expected %016llx\n",op,(unsigned long long)aa,(unsigned long long)bb,(unsigned long long)db(got),(unsigned long long)db(want));++fail;}
}
int main(void){
  static const uint32_t fvec[]={0u,0x80000000u,1u,0x80000001u,0x7fffffu,0x807fffffu,0x00800000u,0x80800000u,0x3f800000u,0xbf800000u,0x3f7fffffu,0x3f800001u,0x7f7fffffu,0xff7fffffu,0x7f800000u,0xff800000u,0x7fc00000u,0x7fa00000u,0x4f800000u,0xcf800000u};
  static const uint64_t dvec[]={0u,UINT64_C(0x8000000000000000),1u,UINT64_C(0x8000000000000001),UINT64_C(0x000fffffffffffff),UINT64_C(0x0010000000000000),UINT64_C(0x3ff0000000000000),UINT64_C(0xbff0000000000000),UINT64_C(0x7fefffffffffffff),UINT64_C(0x7ff0000000000000),UINT64_C(0x7ff8000000000000),UINT64_C(0xfff0000000000000)};
  for(unsigned i=0;i<sizeof(fvec)/4;i++)for(unsigned j=0;j<sizeof(fvec)/4;j++){
    uint32_t a=fvec[i],b=fvec[j];float x=ff(a),y=ff(b);
    cf("fadd",a,b,__aeabi_fadd(x,y),x+y);
    cf("fsub",a,b,__aeabi_fsub(x,y),x-y);
    cf("fmul",a,b,__aeabi_fmul(x,y),x*y);
    cf("fdiv",a,b,__aeabi_fdiv(x,y),x/y);
    if(__aeabi_fcmpge(x,y)!=(x>=y))fail++;
  }
  for(unsigned i=0;i<sizeof(dvec)/8;i++)for(unsigned j=0;j<sizeof(dvec)/8;j++){
    uint64_t a=dvec[i],b=dvec[j];double x=dd(a),y=dd(b);
    cd("dmul",a,b,__aeabi_dmul(x,y),x*y);
  }
  for(unsigned i=0;i<350000;i++){
    uint32_t a=(uint32_t)next64(),b=(uint32_t)next64();float x=ff(a),y=ff(b);
    cf("fadd",a,b,__aeabi_fadd(x,y),x+y);
    cf("fsub",a,b,__aeabi_fsub(x,y),x-y);
    cf("fmul",a,b,__aeabi_fmul(x,y),x*y);
    cf("fdiv",a,b,__aeabi_fdiv(x,y),x/y);
    if(__aeabi_fcmpge(x,y)!=(x>=y))fail++;
    uint64_t da=next64(),dbb=next64();double dx=dd(da),dy=dd(dbb);
    cd("dmul",da,dbb,__aeabi_dmul(dx,dy),dx*dy);
    uint32_t ui=(uint32_t)next64();
    cf("ui2f",ui,0,__aeabi_ui2f(ui),(float)ui);
    if(!dmatch(__aeabi_ui2d(ui),(double)ui))fail++;
    /* C conversions are defined in these ranges only. */
    if(isfinite(x)&&x>=0.0f&&x<4294967296.0f){unsigned want=(unsigned)x;
      if(__aeabi_f2uiz(x)!=want)fail++;}
    if(isfinite(dx)&&dx >= -0x1.0p63 && dx < 0x1.0p63){long long want=(long long)dx;
      if(__aeabi_d2lz(dx)!=want)fail++;}
  }
  /* Test dense values around the critical rounding boundaries. */
  for(uint32_t i=0;i<100000;i++){
    uint32_t u=UINT_MAX-i;
    cf("ui2f-max",u,0,__aeabi_ui2f(u),(float)u);
    uint32_t a=0x00800000u+i,b=0x007fffffu-i%65536u;
    cf("fadd-tiny",a,b,__aeabi_fadd(ff(a),ff(b)),ff(a)+ff(b));
    cf("fsub-tiny",a,b,__aeabi_fsub(ff(a),ff(b)),ff(a)-ff(b));
  }
  if(fail) { fprintf(stderr,"SOFTFLOAT FAILURES: %d\n",fail); return 1; }
  puts("Stage33 softfloat: random 350000, directed edge cases, dense boundary inputs: PASS");
  return 0;
}
