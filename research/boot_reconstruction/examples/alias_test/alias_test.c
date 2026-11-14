#include "alias_test.h"
#include "font5x7.h"

/* The test only reads the uncached alias.  The ordinary MMI loader writes this
 * source block as part of the application image before entering the program. */
const uint32_t spmp_alias_seed[SPMP_ALIAS_WORDS]
    __attribute__((aligned(32),used))={
        0x53504d50u,0x414c4941u,0x53544553u,0x54000001u,
        0x01234567u,0x89abcdefu,0x55aa33ccu,0xc35a9e71u
    };
struct alias_test_record spmp_alias_record;
static const char log_path[]="/fat20a0/hda0/SPMPALI.CSV";

static uint32_t length(const char *s){uint32_t n=0;while(s[n])n++;return n;}
static char *copy(char *p,const char *s){while(*s)*p++=*s++;return p;}
static char *hex32(char *p,uint32_t n)
{static const char h[]="0123456789ABCDEF";for(unsigned i=0;i<8;i++)*p++=h[(n>>(28-4*i))&15];return p;}
static int rect(const struct input_services *io,int x,int y,unsigned w,unsigned h)
{const struct mmi_rect_s16 r={(int16_t)x,(int16_t)y,(uint16_t)w,(uint16_t)h};return io->graphics.fill(&r)==0;}
static int color(const struct input_services *io,unsigned r,unsigned g,unsigned b)
{const struct mmi_rgb888 c={(uint8_t)r,(uint8_t)g,(uint8_t)b};return io->graphics.set_color(&c)==0;}
static const unsigned char *glyph(char c)
{if(c>='A'&&c<='Z')return input_letters[(unsigned)(c-'A')];if(c>='0'&&c<='9')return input_digits[(unsigned)(c-'0')];return 0;}
static int text(const struct input_services *io,int x,int y,const char *s)
{
 for(;*s;s++,x+=12){
  const unsigned char *g=glyph(*s);if(!g)continue;
  for(unsigned row=0;row<7;row++)for(unsigned col=0;col<5;col++)
   if((g[row]&(16u>>col))&&!rect(io,x+(int)col*2,y+(int)row*2,2,2))return 0;
 }
 return 1;
}
static int line(const struct input_services *io,int y,const char *s)
{return color(io,0,0,0)&&rect(io,4,y,312,16)&&color(io,232,240,248)&&text(io,4,y,s);}
static int number_line(const struct input_services *io,int y,const char *label,uint32_t value)
{char b[32];char *p=copy(b,label);p=hex32(p,value);*p=0;return line(io,y,b);}

static void append_log(const struct input_services *io,const char *row)
{
 uint32_t n=length(row);int32_t fd=io->open(log_path,0xau,0600u);
 if(fd<0){spmp_alias_record.log_status=1;spmp_alias_record.log_errno=io->error_number();return;}
 if(io->seek(fd,0,2)<0){spmp_alias_record.log_status=2;spmp_alias_record.log_errno=io->error_number();}
 uint32_t done=0;
 for(unsigned tries=0;!spmp_alias_record.log_status&&done<n&&tries<8;tries++){
  int32_t written=io->write(fd,row+done,n-done);
  if(written<=0||(uint32_t)written>n-done){spmp_alias_record.log_status=3;spmp_alias_record.log_errno=written<0?io->error_number():0;break;}
  done+=(uint32_t)written;
 }
 if(!spmp_alias_record.log_status&&done!=n)spmp_alias_record.log_status=3;
 if(io->close(fd)<0&&!spmp_alias_record.log_status){spmp_alias_record.log_status=4;spmp_alias_record.log_errno=io->error_number();}
}

static void log_result(const struct input_services *io)
{
 char row[160];char *p=copy(row,"ALIAS_V1,");p=hex32(p,spmp_alias_record.source);*p++=',';
 p=hex32(p,spmp_alias_record.alias);*p++=',';p=hex32(p,spmp_alias_record.result);*p++=',';
 p=hex32(p,spmp_alias_record.mismatch_mask);
 for(unsigned i=0;i<SPMP_ALIAS_WORDS;i++){*p++=',';p=hex32(p,spmp_alias_record.observed[i]);}
 *p++='\n';*p=0;append_log(io,row);
}

uint32_t alias_test_run(const struct input_services *io)
{
 if(!io||!io->delay||!io->open||!io->write||!io->seek||!io->close||!io->error_number)return 2;
 struct probe_oem_adapter adapter={&io->graphics};
 if(probe_oem_operations()->begin(&adapter))return 2;
 uint16_t w=0,h=0;if(io->graphics.dimensions(&w,&h)||w!=320||h!=240)return 2;
 struct mmi_rgb888 saved;if(io->graphics.get_color(&saved))return 2;
 spmp_alias_record.source=(uint32_t)(uintptr_t)spmp_alias_seed;
 spmp_alias_record.alias=spmp_alias_record.source|SPMP_ALIAS_BIT;
 if(spmp_alias_record.source>=SPMP_ALIAS_BIT||
    spmp_alias_record.source>SPMP_ALIAS_BIT-sizeof(spmp_alias_seed)){
  spmp_alias_record.result=ALIAS_TEST_BAD_SOURCE;
 }else{
  const volatile uint32_t *uncached=(const volatile uint32_t *)(uintptr_t)spmp_alias_record.alias;
  for(unsigned i=0;i<SPMP_ALIAS_WORDS;i++){
   uint32_t value=uncached[i];spmp_alias_record.observed[i]=value;
   if(value!=spmp_alias_seed[i])spmp_alias_record.mismatch_mask|=1u<<i;
  }
  spmp_alias_record.result=spmp_alias_record.mismatch_mask?ALIAS_TEST_MISMATCH:ALIAS_TEST_PASS;
 }
 log_result(io);
 int ok=color(io,0,0,0)&&rect(io,0,0,320,240)&&color(io,232,240,248)&&
        text(io,4,4,"SPMP ALIAS TEST")&&
        number_line(io,34,"SOURCE ",spmp_alias_record.source)&&
        number_line(io,54,"ALIAS  ",spmp_alias_record.alias)&&
        number_line(io,74,"MASK   ",spmp_alias_record.mismatch_mask)&&
        line(io,104,spmp_alias_record.result==ALIAS_TEST_PASS?"ALIAS READ PASS":
                    spmp_alias_record.result==ALIAS_TEST_BAD_SOURCE?"SOURCE RANGE FAIL":"ALIAS READ FAIL")&&
        line(io,130,spmp_alias_record.log_status?"SD LOG ERROR":"SD LOG OK")&&
        line(io,164,"READ ONLY NO FLASH")&&line(io,188,"RETURNING TO MMI")&&
        io->graphics.paint()==0;
 if(!ok)spmp_alias_record.result=ALIAS_TEST_GRAPHICS_ERROR;
 io->delay(50,0);
 if(io->graphics.set_color(&saved))return 2;
 return 0;
}
