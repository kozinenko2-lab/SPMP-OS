#include "spmp_audio_v5.h"
#include "spmp_amr_opencore.h"
#include <stdio.h>
#include <stdlib.h>
static int rd(void*u,void*d,unsigned n){int r=(int)fread(d,1,n,(FILE*)u);return r?r:ferror((FILE*)u)?-1:0;}
static int sk(void*u,uint64_t off){return off>2147483647u?-1:fseek((FILE*)u,(long)off,SEEK_SET);}
static int64_t sz(void*u){FILE*f=(FILE*)u;long pos=ftell(f),len;fseek(f,0,SEEK_END);len=ftell(f);fseek(f,pos,SEEK_SET);return len;}
int main(int argc,char**argv){sp_audio_player *p;sp_audio_io io;sp_audio_external_backend b;sp_amr_opencore_ctx ctx;FILE *in,*out;int16_t pcm[320];size_t n;int rc,i,fmt;
 if(argc!=4)return 1;in=fopen(argv[1],"rb");out=fopen(argv[2],"wb");if(!in||!out)return 2;
 fmt=atoi(argv[3]);p=calloc(1,sp_audio_player_size());if(!p)return 2;
 sp_amr_opencore_make(&b,&ctx,fmt);io.user=in;io.read=rd;io.seek=sk;io.size=sz;
 rc=sp_audio_open_with_backend(p,io,fmt,(void*)&ctx,1,&b);if(rc){fprintf(stderr,"open=%d\n",rc);return 3;}
 for(i=0;i<100000;i++){rc=sp_audio_decode(p,pcm,320,&n);if(rc==SP_AUD_EOF)break;if(rc){fprintf(stderr,"dec=%d\n",rc);return 4;}if(fwrite(pcm,2,n,out)!=n)return 5;}
 sp_audio_close(p);free(p);fclose(in);fclose(out);return 0;}
