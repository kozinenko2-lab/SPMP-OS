#include "spmp_flac_lite.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char**argv){
 FILE *f,*out;long n;uint8_t *data;void*scratch;int16_t *pcm;spmp_flac d;int err;size_t frames=0,total=0;
 if(argc!=3)return 2;
 f=fopen(argv[1],"rb");if(!f)return 3;
 if(fseek(f,0,SEEK_END)){fclose(f);return 3;}n=ftell(f);
 if(n<=0||n>8000000){fclose(f);return 3;}
 rewind(f);data=(uint8_t*)malloc((size_t)n);
 if(!data){fclose(f);return 3;}
 if(fread(data,1,(size_t)n,f)!=(size_t)n){fclose(f);free(data);return 3;}
 fclose(f);
 err=spmp_flac_open(&d,data,(size_t)n);
 if(err){fprintf(stderr,"open error=%d\n",err);free(data);return 4;}
 fprintf(stderr,"FLAC %u Hz %u ch %u bit block %u\n",d.sample_rate,d.channels,d.bits_per_sample,d.max_block);
 scratch=calloc(1,spmp_flac_work_size(d.max_block));
 pcm=(int16_t*)malloc((size_t)d.max_block*d.channels*sizeof(int16_t));
 out=fopen(argv[2],"wb");if(!scratch||!pcm||!out)return 3;
 for(;;){
  err=spmp_flac_decode_frame(&d,scratch,spmp_flac_work_size(d.max_block),pcm,
      (size_t)d.max_block*d.channels,&frames);
  if(err==SPFL_EOF)break;
  if(err){fprintf(stderr,"decode error=%d after %zu at offset=%zu\n",err,total,d.cursor);return 5;}
  if(fwrite(pcm,sizeof(*pcm),(size_t)frames*d.channels,out)!=(size_t)frames*d.channels)return 6;
  total+=frames;if(total>1000000)return 6;
 }
 fprintf(stderr,"decoded frames=%zu\n",total);
 fclose(out);free(pcm);free(scratch);free(data);return 0;
}
