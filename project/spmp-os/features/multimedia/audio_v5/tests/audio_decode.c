#include "spmp_audio_v5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static int read_cb(void*u,void*p,unsigned n){FILE*f=u;size_t k=fread(p,1,n,f);if(k)return (int)k;return ferror(f)?-1:0;}
static int seek_cb(void*u,uint64_t at){return at>2147483647u?-1:fseek(u,(long)at,SEEK_SET);}
static int64_t size_cb(void*u){long old=ftell(u),len;if(old<0||fseek(u,0,SEEK_END))return -1;len=ftell(u);fseek(u,old,SEEK_SET);return len;}
int main(int argc,char**argv){sp_audio_player *p;sp_audio_io io;sp_audio_info info;size_t n;int16_t pcm[2*65536];uint8_t *work;FILE *in,*out;int r,i;unsigned frames=0;
 if(argc<3){fprintf(stderr,"usage: audio_decode input output.s16 [seek_ms]\n");return 1;}
 in=fopen(argv[1],"rb");out=fopen(argv[2],"wb");if(!in||!out)return 2;
 io.user=in;io.read=read_cb;io.seek=seek_cb;io.size=size_cb;
 p=calloc(1,sp_audio_player_size());work=calloc(1,256*1024);if(!p||!work)return 2;
 r=sp_audio_open(p,io,SP_FMT_AUTO,work,256*1024);if(r){fprintf(stderr,"open error %d\n",r);return 3;}
 info=sp_audio_get_info(p);fprintf(stderr,"format=%u rate=%u channels=%u frames=%llu\n",info.format,info.rate,info.channels,(unsigned long long)info.total_frames);
 if(argc==4){r=sp_audio_seek_ms(p,(uint64_t)strtoull(argv[3],0,10));if(r){fprintf(stderr,"seek %d\n",r);return 4;}}
 for(i=0;i<200000;i++){n=0;r=sp_audio_decode(p,pcm,65536,&n);if(r==SP_AUD_EOF)break;
  if(r){fprintf(stderr,"decode error %d frame=%u\n",r,frames);return 5;}
  if(!n){fprintf(stderr,"zero samples\n");return 6;}
  if(fwrite(pcm,info.channels*sizeof(int16_t),n,out)!=n)return 6;
  frames+=(unsigned)n;
 }
 fprintf(stderr,"decoded frames=%u\n",frames);sp_audio_close(p);free(work);free(p);fclose(in);fclose(out);return 0;
}
