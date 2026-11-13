/* SPMP-OS Stage29: freestanding byte/string routines for ARM926EJ-S.
 * These functions require only ordinary readable/writable RAM.
 * No global state, malloc, operating-system calls, or MMIO accesses.
 */
#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dst, const void *src, size_t count) {
    unsigned char *d=(unsigned char *)dst;
    const unsigned char *s=(const unsigned char *)src;
    for (size_t i=0; i<count; ++i) d[i]=s[i];
    return dst;
}
void *memmove(void *dst, const void *src, size_t count) {
    unsigned char *d=(unsigned char *)dst;
    const unsigned char *s=(const unsigned char *)src;
    if (count == 0 || dst == src) return dst;
    /* Integer address ordering is appropriate on the flat physical ARM target.
       Only compare address distance when dst lies above src. */
    if ((uintptr_t)d > (uintptr_t)s && (uintptr_t)d-(uintptr_t)s < count) {
        while (count) { --count; d[count]=s[count]; }
    } else {
        for (size_t i=0; i<count; ++i) d[i]=s[i];
    }
    return dst;
}
void *memset(void *dst, int value, size_t count) {
    unsigned char *d=(unsigned char *)dst;
    for (size_t i=0; i<count; ++i) d[i]=(unsigned char)value;
    return dst;
}
int memcmp(const void *lhs, const void *rhs, size_t count) {
    const unsigned char *a=lhs,*b=rhs;
    for (size_t i=0; i<count; ++i) if (a[i]!=b[i]) return (int)a[i]-(int)b[i];
    return 0;
}
void *memchr(const void *s, int value, size_t count) {
    const unsigned char *p=s;
    for (size_t i=0; i<count; ++i) if (p[i]==(unsigned char)value) return (void *)(p+i);
    return NULL;
}
size_t strlen(const char *s) { size_t n=0; while (s[n]) ++n; return n; }
int strcmp(const char *a, const char *b) {
    while (*a && (unsigned char)*a==(unsigned char)*b) { ++a; ++b; }
    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}
int strncmp(const char *a, const char *b, size_t count) {
    for(size_t i=0;i<count;++i) {
        unsigned char x=(unsigned char)a[i], y=(unsigned char)b[i];
        if(x!=y) return (int)x-(int)y;
        if(x==0) return 0;
    }
    return 0;
}
char *strcpy(char *dst, const char *src) {
    char *p=dst;
    do { *p=*src; } while (*src++ && ++p);
    return dst;
}
char *strcat(char *dst, const char *src) {
    char *p=dst+strlen(dst);
    while ((*p++=*src++)!=0) {}
    return dst;
}
char *strrchr(const char *s, int c) {
    const char *last=NULL;
    unsigned char ch=(unsigned char)c;
    do { if((unsigned char)*s==ch) last=s; } while (*s++);
    return (char *)last;
}
/* AAPCS EABI memory helpers. AEABI memset argument order is (dst, n, c). */
void __aeabi_memcpy(void *d, const void *s, size_t n) { (void)memcpy(d,s,n); }
void __aeabi_memcpy4(void *d, const void *s, size_t n) { (void)memcpy(d,s,n); }
void __aeabi_memcpy8(void *d, const void *s, size_t n) { (void)memcpy(d,s,n); }
void __aeabi_memmove(void *d, const void *s, size_t n) { (void)memmove(d,s,n); }
void __aeabi_memmove4(void *d, const void *s, size_t n) { (void)memmove(d,s,n); }
void __aeabi_memset(void *d, size_t n, int c) { (void)memset(d,c,n); }
void __aeabi_memset4(void *d, size_t n, int c) { (void)memset(d,c,n); }
void __aeabi_memclr(void *d, size_t n) { (void)memset(d,0,n); }
void __aeabi_memclr4(void *d, size_t n) { (void)memset(d,0,n); }
