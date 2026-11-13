/* Stage32: narrowly scoped gpSP freestanding printf and sscanf.
 * printf forwards to a real HAL log callback (never invented UART/MMIO).
 * sscanf supports the conversions required by gpSP cheat parsing.
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "spmp_gpsp_services.h"

static spmp_gpsp_log_sink log_sink;
static void *log_ctx;
void spmp_gpsp_bind_log(spmp_gpsp_log_sink sink, void *ctx) {
    log_sink = sink;
    log_ctx = sink ? ctx : NULL;
}

struct log_state { char data[128]; unsigned count; int total; };
static void log_flush(struct log_state *s) {
    if (s->count) { log_sink(log_ctx,s->data,s->count); s->count=0; }
}
static void log_char(struct log_state *s, char c) {
    if (s->count == sizeof(s->data)) log_flush(s);
    s->data[s->count++] = c;
    ++s->total;
}
static int is_digit(char c) { return c>='0' && c<='9'; }
static int format_uint(struct log_state *s, uint32_t val, unsigned base,
                       int width, char pad, int upper, int negative) {
    char tmp[40]; int n=0;
    const char *digits=upper?"0123456789ABCDEF":"0123456789abcdef";
    do { tmp[n++]=digits[val%base]; val/=base; } while (val);
    int len=n+negative;
    if (negative && pad=='0') log_char(s,'-');
    for (int i=len;i<width;i++) log_char(s,pad);
    if (negative && pad!='0') log_char(s,'-');
    while (n) log_char(s,tmp[--n]);
    return 0;
}
int printf(const char *fmt, ...) {
    if (!fmt || !log_sink) return -1;
    struct log_state s={{0},0,0};
    va_list ap; va_start(ap,fmt);
    while (*fmt) {
        if (*fmt!='%') {log_char(&s,*fmt++);continue;}
        ++fmt;
        if (*fmt=='%') {log_char(&s,'%');++fmt;continue;}
        char pad=' '; int width=0;
        if (*fmt=='0') {pad='0';++fmt;}
        while (is_digit(*fmt)) {
            width=width*10+(*fmt++-'0');
            if (width>256) {va_end(ap);return -1;}
        }
        /* Existing gpSP debug call sites pass 32-bit ints only. */
        char f=*fmt++;
        if (f=='s') {
            const char *p=va_arg(ap,const char *);
            if (!p) p="(null)";
            unsigned len=0; while (p[len]) ++len;
            for (int i=(int)len;i<width;i++) log_char(&s,' ');
            while (*p) log_char(&s,*p++);
        } else if (f=='c') {
            for (int i=1;i<width;i++) log_char(&s,' ');
            log_char(&s,(char)va_arg(ap,int));
        } else if (f=='d' || f=='i') {
            int32_t n=va_arg(ap,int);
            uint32_t mag=n<0?(uint32_t)(-(int64_t)n):(uint32_t)n;
            format_uint(&s,mag,10,width,pad,0,n<0);
        } else if (f=='u' || f=='x' || f=='X') {
            uint32_t n=va_arg(ap,unsigned);
            format_uint(&s,n,f=='u'?10:16,width,pad,f=='X',0);
        } else {
            va_end(ap); return -1; /* No silently fabricated formatting. */
        }
    }
    va_end(ap);
    log_flush(&s);
    return s.total;
}
static int hex_digit(char c) {
    if (c>='0'&&c<='9') return c-'0';
    if (c>='a'&&c<='f') return c-'a'+10;
    if (c>='A'&&c<='F') return c-'A'+10;
    return -1;
}
static int space(char c) { return c==' ' || (c>='\t' && c<='\r'); }
/* Supports %x, %hx, %u, %d and literal/whitespace matching.
 * No reliance on the platform libc; the gpSP cheat reader uses "%08x %04hx".
 */
int sscanf(const char *src, const char *fmt, ...) {
    if (!src || !fmt) return -1;
    va_list args; va_start(args,fmt);
    int assigned=0, any_input=0;
    const char *s=src;
    while (*fmt) {
        if (space(*fmt)) {
            while (space(*fmt)) ++fmt;
            while (space(*s)) { ++s;any_input=1; }
            continue;
        }
        if (*fmt!='%') {
            if (*s!=*fmt) break;
            ++s; ++fmt; any_input=1; continue;
        }
        ++fmt;
        if (*fmt=='%') {
            if (*s!='%') break;
            ++s;++fmt;any_input=1;continue;
        }
        int width=0;
        while(is_digit(*fmt)) {width=width*10+(*fmt++-'0');if(width>256) {va_end(args);return assigned;}}
        if (!width) width=256;
        int half=0;
        if (*fmt=='h') {half=1;++fmt;}
        char type=*fmt++;
        if (!(type=='x'||type=='X'||type=='u'||type=='d')) {va_end(args);return assigned;}
        while (space(*s)) {++s;any_input=1;}
        const char *start=s;
        int neg=0, base=(type=='x'||type=='X')?16:10;
        if ((*s=='+'||*s=='-') && width>0) {neg=*s=='-'; ++s;--width;}
        if (base==16 && width>=3 && s[0]=='0' && (s[1]=='x'||s[1]=='X') && hex_digit(s[2])>=0) { s+=2;width-=2; }
        uint32_t value=0; int digits=0;
        while (width && *s) {
            int d=hex_digit(*s);
            if (d<0 || d>=base) break;
            value=value*(unsigned)base+(unsigned)d;
            ++s;--width;++digits;
        }
        if (!digits) {s=start;break;}
        if (neg) value=0u-value;
        if (half) {unsigned short *dest=va_arg(args,unsigned short *); if (dest) *dest=(unsigned short)value;}
        else if (type=='d') {int *dest=va_arg(args,int *); if(dest) *dest=(int32_t)value;}
        else {unsigned *dest=va_arg(args,unsigned *); if(dest) *dest=value;}
        ++assigned;any_input=1;
    }
    va_end(args);
    return (!assigned && !any_input && !*s) ? -1 : assigned;
}
