/* SPMP-OS Stage33 - freestanding ARM EABI soft float subset (IEEE-754 RNE).
 * Integer-only; ARMv5TE has no hardware IEEE floating point unit.
 * Designed for gpSP's ten currently unresolved __aeabi_* entry points.
 * No status-flag (fenv) emulation; floating-to-int out-of-range policy is
 * clamping, and NaN-to-int yields zero. See README_STAGE33_RU.md.
 * Copyright (c) 2026 SPMP-OS contributors. SPDX-License-Identifier: MIT.
 */
#include <stdint.h>
#include <limits.h>

typedef union { float f; uint32_t u; } sf32;
typedef union { double d; uint64_t u; } sf64;
static uint32_t fbits(float v) { sf32 b; b.f = v; return b.u; }
static float fromf(uint32_t u) { sf32 b; b.u = u; return b.f; }
static uint64_t dbits(double v) { sf64 b; b.d = v; return b.u; }
static double fromd(uint64_t u) { sf64 b; b.u = u; return b.d; }
static uint32_t jam32(uint32_t v, unsigned n) {
    if (!n) return v;
    if (n >= 32) return v != 0;
    return (v >> n) | ((v << (32-n)) != 0);
}
static int is_fnan(uint32_t v) { return ((v & 0x7f800000u) == 0x7f800000u) && (v & 0x007fffffu); }
static int is_dnan(uint64_t v) { return ((v & UINT64_C(0x7ff0000000000000)) == UINT64_C(0x7ff0000000000000)) && (v & UINT64_C(0xfffffffffffff)); }
static uint32_t fnan(uint32_t v) { return is_fnan(v) ? (v | 0x00400000u) : 0x7fc00000u; }
static uint64_t dnan(uint64_t v) { return is_dnan(v) ? (v | UINT64_C(0x0008000000000000)) : UINT64_C(0x7ff8000000000000); }
/* Returns a 24-bit normal significand even for subnormal inputs. */
static void fdecode(uint32_t v, int *e, uint32_t *m) {
    int exp = (int)((v >> 23) & 255);
    uint32_t frac = v & 0x7fffffu;
    if (exp) { *e = exp - 127; *m = (1u<<23)|frac; }
    else {
        int k = -126;
        while ((frac & (1u<<23)) == 0) { frac <<= 1; --k; }
        *e = k; *m = frac;
    }
}
static uint32_t fpack(uint32_t sign, int exp, uint32_t sig) {
    if (!sig) return sign;
    if (exp < -126) sig = jam32(sig, (unsigned)(-126-exp)), exp = -126;
    uint32_t m = sig >> 3, bits = sig & 7u;
    if (bits > 4u || (bits == 4u && (m & 1u))) ++m;
    if (m >= (1u << 24)) { m >>= 1; ++exp; }
    if (exp > 127) return sign | 0x7f800000u;
    if (exp == -126 && m < (1u << 23)) return sign | m;
    return sign | ((uint32_t)(exp + 127) << 23) | (m & 0x7fffffu);
}
static uint32_t faddbits(uint32_t a, uint32_t b) {
    if (is_fnan(a)) return fnan(a);
    if (is_fnan(b)) return fnan(b);
    uint32_t amag = a & 0x7fffffffu, bmag = b & 0x7fffffffu;
    if (amag == 0x7f800000u) {
        if (bmag == 0x7f800000u && ((a ^ b)&0x80000000u)) return 0x7fc00000u;
        return a;
    }
    if (bmag == 0x7f800000u) return b;
    if (!amag && !bmag) return (a & b & 0x80000000u); /* -0 + -0 => -0 */
    if (!amag) return b;
    if (!bmag) return a;
    uint32_t sa = a & 0x80000000u, sb = b & 0x80000000u;
    if (amag < bmag) { uint32_t t=a; a=b; b=t; t=sa; sa=sb; sb=t; }
    int ea, eb; uint32_t ma, mb;
    fdecode(a,&ea,&ma); fdecode(b,&eb,&mb);
    uint32_t qa = ma << 3, qb = jam32(mb << 3,(unsigned)(ea-eb));
    if (sa == sb) {
        uint32_t sum = qa + qb;
        if (sum >= (1u<<27)) { sum = jam32(sum,1); ++ea; }
        return fpack(sa,ea,sum);
    }
    uint32_t diff = qa - qb;
    if (!diff) return 0;  /* exactly cancelled => +0 in RNE */
    while (diff < (1u<<26)) { diff <<= 1; --ea; }
    return fpack(sa,ea,diff);
}
static uint32_t fmulbits(uint32_t a, uint32_t b) {
    uint32_t sign=(a^b)&0x80000000u;
    uint32_t amag=a&0x7fffffffu,bmag=b&0x7fffffffu;
    if (is_fnan(a)) return fnan(a);
    if (is_fnan(b)) return fnan(b);
    if ((amag==0x7f800000u && !bmag) || (bmag==0x7f800000u && !amag)) return 0x7fc00000u;
    if (amag==0x7f800000u || bmag==0x7f800000u) return sign|0x7f800000u;
    if (!amag || !bmag) return sign;
    int ea,eb; uint32_t ma,mb;
    fdecode(a,&ea,&ma); fdecode(b,&eb,&mb);
    uint64_t p = (uint64_t)ma * mb;
    unsigned shift = (p & (UINT64_C(1)<<47)) ? 21u:20u;
    uint32_t q=(uint32_t)(p>>shift) | ((p & ((UINT64_C(1)<<shift)-1))!=0);
    return fpack(sign,ea+eb+(int)shift-20,q);
}
static uint32_t fdivbits(uint32_t a, uint32_t b) {
    uint32_t sign=(a^b)&0x80000000u, amag=a&0x7fffffffu,bmag=b&0x7fffffffu;
    if (is_fnan(a)) return fnan(a);
    if (is_fnan(b)) return fnan(b);
    if ((amag==0x7f800000u && bmag==0x7f800000u) || (!amag && !bmag)) return 0x7fc00000u;
    if (amag==0x7f800000u || !bmag) return sign|0x7f800000u;
    if (!amag || bmag==0x7f800000u) return sign;
    int ea,eb; uint32_t ma,mb;
    fdecode(a,&ea,&ma); fdecode(b,&eb,&mb);
    int exponent=ea-eb;
    unsigned shift = 26;
    if (ma < mb) { --exponent; shift=27; }
    uint64_t numerator=(uint64_t)ma<<shift;
    uint32_t q=(uint32_t)(numerator/mb);
    if ((numerator%mb)!=0) q|=1;
    return fpack(sign,exponent,q);
}
static void ddecode(uint64_t bits,int *e,uint64_t *m) {
    int exp=(int)((bits>>52)&2047);
    uint64_t frac=bits&UINT64_C(0x000fffffffffffff);
    if (exp) { *e=exp-1023; *m=frac|UINT64_C(0x0010000000000000); }
    else {
        int k=-1022;
        while (!(frac & UINT64_C(0x0010000000000000))) { frac<<=1; --k; }
        *e=k; *m=frac;
    }
}
static uint64_t jam64(uint64_t v,unsigned s) {
    if (!s) return v;
    if (s>=64) return v!=0;
    return (v>>s) | ((v << (64-s)) != 0);
}
static uint64_t dpack(uint64_t sign,int exp,uint64_t q) {
    if (!q) return sign;
    if (exp < -1022) q=jam64(q,(unsigned)(-1022-exp)), exp=-1022;
    uint64_t m=q>>3, rem=q&7;
    if (rem>4 || (rem==4 && (m&1))) ++m;
    if (m >= (UINT64_C(1)<<53)) { m>>=1; ++exp; }
    if (exp>1023) return sign | UINT64_C(0x7ff0000000000000);
    if (exp==-1022 && m < (UINT64_C(1)<<52)) return sign | m;
    return sign | ((uint64_t)(exp+1023)<<52) | (m & UINT64_C(0xfffffffffffff));
}
/* Exact unsigned 53x53 -> 106 bit product, represented as {hi64,lo64}.
   Each multiply below is an actual 32x32 -> 64 multiply on ARMv5. */
static void mul53(uint64_t a,uint64_t b,uint64_t *hi,uint64_t *lo) {
    uint32_t al=(uint32_t)a, ah=(uint32_t)(a>>32);
    uint32_t bl=(uint32_t)b, bh=(uint32_t)(b>>32);
    uint64_t p0=(uint64_t)al*bl, p1=(uint64_t)ah*bl;
    uint64_t p2=(uint64_t)al*bh, p3=(uint64_t)ah*bh;
    uint64_t mid=(p0>>32)+(uint32_t)p1+(uint32_t)p2;
    *lo=(p0&UINT64_C(0xffffffff))|(mid<<32);
    *hi=p3+(p1>>32)+(p2>>32)+(mid>>32);
}
static uint64_t dmulbits(uint64_t a,uint64_t b) {
    uint64_t sign=(a^b)&UINT64_C(0x8000000000000000);
    uint64_t amag=a&UINT64_C(0x7fffffffffffffff), bmag=b&UINT64_C(0x7fffffffffffffff);
    if(is_dnan(a)) return dnan(a);
    if(is_dnan(b)) return dnan(b);
    const uint64_t inf=UINT64_C(0x7ff0000000000000);
    if((amag==inf && !bmag)||(bmag==inf && !amag)) return UINT64_C(0x7ff8000000000000);
    if(amag==inf||bmag==inf) return sign|inf;
    if(!amag||!bmag) return sign;
    int ea,eb; uint64_t ma,mb,hi,lo;
    ddecode(a,&ea,&ma); ddecode(b,&eb,&mb);
    mul53(ma,mb,&hi,&lo);
    /* Normal product occupies bit104 or bit105. Since hi is product>>64,
       this is bit40 or bit41 in the high word. */
    unsigned top=(hi & (UINT64_C(1)<<41)) ? 105u:104u;
    unsigned shift=top-55; /* 49 or 50; 56 bits incl guard/round/sticky */
    uint64_t q=(hi<<(64-shift))|(lo>>shift);
    if (lo & ((UINT64_C(1)<<shift)-1)) q|=1;
    return dpack(sign,ea+eb+(int)top-104,q);
}
float __aeabi_fadd(float a,float b) { return fromf(faddbits(fbits(a),fbits(b))); }
float __aeabi_fsub(float a,float b) { return fromf(faddbits(fbits(a),fbits(b)^0x80000000u)); }
float __aeabi_fmul(float a,float b) { return fromf(fmulbits(fbits(a),fbits(b))); }
float __aeabi_fdiv(float a,float b) { return fromf(fdivbits(fbits(a),fbits(b))); }
int __aeabi_fcmpge(float a,float b) {
    uint32_t x=fbits(a),y=fbits(b);
    if (is_fnan(x)||is_fnan(y)) return 0;
    if (((x|y)&0x7fffffffu)==0) return 1;
    if ((x^y)&0x80000000u) return !(x&0x80000000u);
    return (x&0x80000000u) ? (x<=y) : (x>=y);
}
double __aeabi_dmul(double a,double b) { return fromd(dmulbits(dbits(a),dbits(b))); }
float __aeabi_ui2f(unsigned a) {
    if(!a) return fromf(0);
    int p=31; while (!(a&(1u<<p))) --p;
    uint32_t sig;
    if(p<=23) sig = a<<(23-p);
    else {
        unsigned sh=(unsigned)(p-23);
        sig=a>>sh;
        uint32_t tail=a&((1u<<sh)-1u),half=1u<<(sh-1);
        if(tail>half||(tail==half&&(sig&1))) ++sig;
        if(sig==(1u<<24)) {sig>>=1;++p;}
    }
    return fromf(((uint32_t)(p+127)<<23)|(sig&0x7fffffu));
}
double __aeabi_ui2d(unsigned a) {
    if(!a) return fromd(0);
    int p=31; while(!(a&(1u<<p))) --p;
    return fromd(((uint64_t)(p+1023)<<52)|(((uint64_t)a<<(52-p))&UINT64_C(0xfffffffffffff)));
}
unsigned __aeabi_f2uiz(float v) {
    uint32_t b=fbits(v),sign=b>>31;
    int ex=(int)((b>>23)&255)-127;
    if(sign || is_fnan(b) || ex<0) return 0;
    if(ex>=32) return UINT_MAX;
    uint32_t m=(b&0x7fffffu)|(1u<<23);
    return ex>=23 ? m<<(ex-23) : m>>(23-ex);
}
long long __aeabi_d2lz(double v) {
    uint64_t b=dbits(v); int sign=(int)(b>>63);
    unsigned ef=(unsigned)((b>>52)&2047);
    if(ef==2047) return (b&UINT64_C(0xfffffffffffff)) ? 0 : (sign?INT64_MIN:INT64_MAX);
    int ex=(int)ef-1023;
    if(ex<0) return 0;
    if(ex>=63) return sign?INT64_MIN:INT64_MAX;
    uint64_t m=(b&UINT64_C(0xfffffffffffff))|UINT64_C(0x10000000000000);
    uint64_t magnitude=ex>=52 ? (m<<(ex-52)) : (m>>(52-ex));
    if(sign) return magnitude>=UINT64_C(0x8000000000000000)?INT64_MIN:-(int64_t)magnitude;
    return magnitude>INT64_MAX?INT64_MAX:(int64_t)magnitude;
}
