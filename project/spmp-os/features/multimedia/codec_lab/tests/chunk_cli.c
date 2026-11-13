/* Example of bounded compressed input with the new decode_chunk() API.
   Host uses malloc for the test file only; the decoder API itself does not. */
#include "spmp_flac_lite.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 FILE *f,*out;long n;uint8_t *file;int16_t *pcm;void *scratch;spmp_flac meta,header_only;
 size_t pos=0,consumed,frames,total=0;int rc;
 if(argc!=3)return 2;
 f=fopen(argv[1],"rb");if(!f)return 2;
 fseek(f,0,SEEK_END);n=ftell(f);rewind(f);
 if(n<10||n>8000000)return 2;
 file=malloc((size_t)n);
 if(!file||fread(file,1,(size_t)n,f)!=(size_t)n)return 2;
 fclose(f);
 if(spmp_flac_open(&meta,file,(size_t)n))return 3;
 if(spmp_flac_open(&header_only,file,meta.first_frame))return 3;
 if(meta.sample_rate!=header_only.sample_rate || meta.max_block!=header_only.max_block)return 3;
 scratch=malloc(spmp_flac_work_size(meta.max_block));
 pcm=malloc((size_t)meta.max_block*meta.channels*2);
 out=fopen(argv[2],"wb");if(!scratch||!pcm||!out)return 3;
 pos=meta.first_frame;
 while(pos<(size_t)n){
  /* Decoder sees only compressed frame window, never the full file. */
  rc=spmp_flac_decode_chunk(&header_only,file+pos,(size_t)n-pos,scratch,
                           spmp_flac_work_size(meta.max_block),pcm,
                           (size_t)meta.max_block*meta.channels,&consumed,&frames);
  if(rc!=0 || !consumed){fprintf(stderr,"chunk decode error %d pos=%zu\n",rc,pos);return 4;}
  pos+=consumed;
  if(fwrite(pcm,2,frames*meta.channels,out)!=frames*meta.channels)return 5;
  total+=frames;
 }
 fprintf(stderr,"chunk frames=%zu\n",total);
 fclose(out);free(file);free(scratch);free(pcm);
 return 0;
}
