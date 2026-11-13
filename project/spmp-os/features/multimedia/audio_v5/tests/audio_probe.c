#include "spmp_audio_v5.h"
#include <stdio.h>
static int rd(void*u,void*p,unsigned n){size_t r=fread(p,1,n,(FILE*)u);return r?(int)r:ferror((FILE*)u)?-1:0;}
static int sk(void*u,uint64_t off){return off>2147483647u?-1:fseek((FILE*)u,(long)off,SEEK_SET);}
static int64_t sz(void*u){FILE*f=(FILE*)u;long o=ftell(f),x;fseek(f,0,SEEK_END);x=ftell(f);fseek(f,o,SEEK_SET);return x;}
int main(int argc,char**argv){sp_audio_info info;sp_audio_io io;int r;FILE*f;if(argc<2)return 1;f=fopen(argv[1],"rb");if(!f)return 2;io.user=f;io.read=rd;io.seek=sk;io.size=sz;r=sp_audio_probe(io,SP_FMT_AUTO,&info);printf("rc=%d fmt=%u codec=%u rate=%u ch=%u br=%u frames=%llu\n",r,info.format,info.codec,info.rate,info.channels,info.bitrate,(unsigned long long)info.total_frames);fclose(f);return r==0?0:3;}
