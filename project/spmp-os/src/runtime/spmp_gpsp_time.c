/* Stage32: gpSP emulated RTC time facade. UTC Unix seconds supplied by HAL;
 * deterministic 2000-01-01 epoch only when no validated hardware RTC exists.
 * Local civil calendar is Gregorian, no time-zone/DST database is assumed.
 */
#include <stdint.h>
#include <time.h>
#include "spmp_gpsp_services.h"

static spmp_gpsp_clock_read clock_reader;
static void *clock_ctx;
static int64_t virtual_epoch=946684800;
static int offset_min=0;

void spmp_gpsp_bind_clock(spmp_gpsp_clock_read reader, void *ctx) {
    clock_reader=reader; clock_ctx=reader?ctx:0;
}
int spmp_gpsp_set_virtual_epoch(int64_t epoch) {
    if (epoch<0 || epoch>2147483647LL) return 0;
    virtual_epoch=epoch;return 1;
}
int spmp_gpsp_set_utc_offset_minutes(int offset_minutes) {
    if(offset_minutes < -840 || offset_minutes > 840) return 0;
    offset_min=offset_minutes;return 1;
}
time_t time(time_t *out) {
    int64_t epoch;
    if (!clock_reader || !clock_reader(clock_ctx,&epoch) || epoch<0 || epoch>2147483647LL)
        epoch=virtual_epoch;
    if(out) *out=(time_t)epoch;
    return (time_t)epoch;
}
static int leap(unsigned y) {return y%4==0 && (y%100!=0 || y%400==0);}
struct tm *localtime(const time_t *stamp) {
    static struct tm result;
    if(!stamp) return 0;
    int64_t sec=(int64_t)*stamp+(int64_t)offset_min*60;
    if(sec<0 || sec > 2147483647LL+840LL*60) return 0;
    uint32_t day=(uint32_t)(sec/86400);
    uint32_t rem=(uint32_t)(sec%86400);
    result.tm_sec=rem%60;rem/=60;
    result.tm_min=rem%60;result.tm_hour=rem/60;
    result.tm_wday=(int)((day+4u)%7u);
    unsigned year=1970;
    while(day >= (unsigned)(leap(year)?366:365)) {
        day-=leap(year)?366:365; ++year;
    }
    result.tm_year=(int)year-1900;
    result.tm_yday=day;
    static const unsigned char days_per_month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    unsigned month=0;
    for(;month<12;month++) {
        unsigned monthdays=days_per_month[month]+(month==1&&leap(year));
        if(day<monthdays) break;
        day-=monthdays;
    }
    result.tm_mon=month;
    result.tm_mday=(int)day+1;
    result.tm_isdst=0;
    return &result;
}
