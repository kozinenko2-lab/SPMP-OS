#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include "spmp_gpsp_services.h"

int printf(const char *, ...);
int sscanf(const char *, const char *, ...);

static char captured[512];
static size_t captured_size;
static void sink(void *ctx, const char *data, size_t count) {
    (void)ctx;
    for(size_t i=0;i<count && captured_size<sizeof(captured);i++)captured[captured_size++]=data[i];
}
static int str_eq(const char *s) {
    size_t i=0;
    while(s[i]&&i<captured_size) {if(s[i]!=captured[i])return 0;++i;}
    return i==captured_size && !s[i];
}
static int fake_clock(void *ctx,int64_t *stamp) {
    *stamp=*(int64_t*)ctx;return 1;
}
#define CHECK(C) do { if(!(C)) return __LINE__; } while(0)
int main(void) {
    unsigned a=0; unsigned short b=0;
    CHECK(sscanf("0123ABcd 00fF","%08x %04hx",&a,&b)==2);
    CHECK(a==0x123ABCDu && b==255u);
    CHECK(sscanf("0xFF 10","%08x %04hx",&a,&b)==2);
    CHECK(a==255 && b==16);
    CHECK(sscanf("bad!","%04hx",&b)==1 && b==0xbad);
    CHECK(sscanf("","%x",&a)==-1);
    CHECK(sscanf("xyz","%x",&a)==0);
    CHECK(sscanf(" ff ","%x",&a)==1 && a==255);
    CHECK(sscanf("ABC DFFF","%3x %4hx",&a,&b)==2 && a==2748 && b==0xdfff);
    CHECK(printf("unbound=%d",1)==-1);
    spmp_gpsp_bind_log(sink,0);
    CHECK(printf("R0=%08x %04X %d %u %s %%",0x12af,0xab, -123,12u,"ok")==29);
    CHECK(str_eq("R0=000012af 00AB -123 12 ok %"));
    captured_size=0;
    CHECK(printf("%05d",-7)==5);
    CHECK(str_eq("-0007"));
    spmp_gpsp_bind_log(0,0);
    time_t n=time(0);
    CHECK((int64_t)n==946684800);
    struct tm *tm=localtime(&n);
    CHECK(tm && tm->tm_year==100 && tm->tm_mon==0 && tm->tm_mday==1);
    CHECK(tm->tm_wday==6 && tm->tm_hour==0 && tm->tm_yday==0);
    CHECK(spmp_gpsp_set_utc_offset_minutes(60));
    n=946684800+86400-1800;
    tm=localtime(&n);
    CHECK(tm && tm->tm_mday==2 && tm->tm_hour==0 && tm->tm_min==30);
    CHECK(spmp_gpsp_set_utc_offset_minutes(0));
    n=951782400;  /* 2000-02-29 UTC */
    tm=localtime(&n);
    CHECK(tm && tm->tm_year==100 && tm->tm_mon==1 && tm->tm_mday==29 && tm->tm_yday==59);
    int64_t t=1609459200; /* 2021-01-01 UTC */
    spmp_gpsp_bind_clock(fake_clock,&t);
    n=time(0);tm=localtime(&n);
    CHECK(n==t && tm && tm->tm_year==121 && tm->tm_mon==0 && tm->tm_mday==1 && tm->tm_wday==5);
    spmp_gpsp_bind_clock(0,0);
    CHECK(spmp_gpsp_set_virtual_epoch(946684801));
    CHECK(time(0)==946684801);
    CHECK(!spmp_gpsp_set_virtual_epoch(-1));
    CHECK(!spmp_gpsp_set_utc_offset_minutes(841));
    return 0;
}
