#include "spmp_media_probe.h"
#include "spmp_flac_lite.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
static int check_stock(void){
 uint32_t a,b;
 CHECK(spmp_stock_mp3_working_ram_recovered(0,&a,&b)==0 && a==18476 && b==0);
 CHECK(spmp_stock_mp3_working_ram_recovered(4608,&a,&b)==0 && a==13868 && b==4608);
 CHECK(spmp_stock_mp3_working_ram_recovered(8192,&a,&b)==0 && a==11564 && b==6912);
 CHECK(spmp_stock_mp3_working_ram_recovered(18432,&a,&b)==0 && a==300 && b==18176);
 CHECK(spmp_stock_mp3_working_ram_recovered(100,NULL,&b)!=0);
 CHECK(spmp_stock_flac_working_ram_recovered()==22728);
 return 0;
}
static int check_headers(void){
 /* MPEG-1 Layer III, 128kbps 44.1kHz stereo (417 bytes) */
 static const uint8_t mp3[]={0xff,0xfb,0x90,0x64};
 /* MPEG-2 Layer III, 64 kbps @22.05 kHz mono (208 bytes) */
 static const uint8_t mp3v2[]={0xff,0xf3,0x80,0xc0};
 /* AAC LC ADTS 44.1kHz stereo, complete 100-byte frame, protection absent */
 static const uint8_t adts[]={0xff,0xf1,0x50,0x80,0x0c,0x9f,0xfc};
 spmp_mp3_frame m;spmp_aac_adts a;size_t off;
 uint8_t twin[1000]={0};
 CHECK(spmp_mp3_header(mp3,4,&m)==0);
 CHECK(m.mpeg_version==1 && m.layer==3 && m.channels==2 &&
       m.sample_rate==44100 && m.bit_rate_bps==128000 && m.frame_bytes==417 &&
       m.samples_per_frame==1152);
 CHECK(spmp_mp3_header(mp3v2,4,&m)==0);
 CHECK(m.mpeg_version==2 && m.channels==1 && m.sample_rate==22050 &&
       m.frame_bytes==208 && m.samples_per_frame==576);
 CHECK(spmp_mp3_header(mp3,2,&m)!=0);
 CHECK(spmp_mp3_header((const uint8_t*)"abcd",4,&m)!=0);
 memcpy(twin+8,mp3,4);memcpy(twin+425,mp3,4);
 CHECK(spmp_mp3_find_frame(twin,sizeof(twin),&off,&m)==0 && off==8);
 twin[425]=0;CHECK(spmp_mp3_find_frame(twin,sizeof(twin),&off,&m)!=0);
 CHECK(spmp_aac_adts_header(adts,sizeof(adts),&a)==0);
 CHECK(a.object_type==2 && a.channels==2 && a.sample_rate==44100 &&
       a.frame_bytes==100 && a.samples_per_frame==1024 && a.header_bytes==7);
 CHECK(spmp_aac_adts_header(adts,3,&a)!=0);
 return 0;
}
static int check_mmpi(void){
 uint8_t h[128]={0};spmp_mmpi m;
 memcpy(h,"MMPi",4);h[4]=128;memcpy(h+16,"MMPd",4);h[20]=36;
 h[35]=0x61;h[36]=12;h[42]=0x10;
 CHECK(spmp_mmpi_inspect(h,sizeof(h),&m)==0 && m.module_family==0x61 && m.module_slot==12);
 h[4]=127;CHECK(spmp_mmpi_inspect(h,sizeof(h),&m)!=0);
 return 0;
}
static int check_crc(void){
 static const uint8_t test[]="123456789";
 CHECK(spmp_flac_crc8(test,9)==0xf4);
 CHECK(spmp_flac_crc16(test,9)==0xfee8);
 return 0;
}
int main(void){
 if(check_stock() || check_headers() || check_mmpi() || check_crc())return 1;
 puts("Probe/compatibility/header/CRC tests passed");return 0;
}
