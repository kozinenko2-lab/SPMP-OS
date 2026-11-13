#include "spmp_media_probe.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 uint8_t p[32768];size_t n,off=0,i;spmp_mp3_frame m;spmp_aac_adts a;FILE *f;
 if(argc!=3)return 2;
 f=fopen(argv[2],"rb");if(!f)return 2;
 n=fread(p,1,sizeof(p),f);fclose(f);
 if(!strcmp(argv[1],"mp3")){
  if(spmp_mp3_find_frame(p,n,&off,&m))return 3;
  printf("MP3 rate=%u bitrate=%u channels=%u frame=%u samples=%u offset=%zu\n",
    m.sample_rate,m.bit_rate_bps,m.channels,m.frame_bytes,m.samples_per_frame,off);
 } else if(!strcmp(argv[1],"aac")){
  for(i=0;i+7<=n;i++)if(!spmp_aac_adts_header(p+i,n-i,&a)){off=i;break;}
  if(i+7>n)return 3;
  printf("AAC-ADTS rate=%u channels=%u frame=%u object=%u samples=%u offset=%zu\n",
   a.sample_rate,a.channels,a.frame_bytes,a.object_type,a.samples_per_frame,off);
 } else return 2;
 return 0;
}
