#include "spmp_wma_asf_v6.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL %s:%d %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
typedef struct {FILE *fp;long sz;unsigned short_max;unsigned seek_count;} io_ctx;
static int r(void*c,void*p,unsigned n){io_ctx*s=c;size_t got=fread(p,1,n>s->short_max?s->short_max:n,s->fp);return (int)got;}
static int sk(void*c,uint64_t pos){io_ctx*s=c;s->seek_count++;if(pos>0x7fffffffu)return -1;return fseek(s->fp,(long)pos,SEEK_SET)==0?0:-1;}
static int64_t sz(void*c){return ((io_ctx*)c)->sz;}
int main(int argc,char**argv){spwma_info inf;spwma_io io;io_ctx ctx;int rc;int i;
 if(argc!=2){fprintf(stderr,"usage %s sample.wma\n",argv[0]);return 2;}
 memset(&ctx,0,sizeof(ctx));ctx.fp=fopen(argv[1],"rb");CHECK(ctx.fp);fseek(ctx.fp,0,SEEK_END);ctx.sz=ftell(ctx.fp);
 io.user=&ctx;io.read=r;io.seek=sk;io.size=sz;
 ctx.short_max=11;rc=spwma_probe(io,&inf);
 fprintf(stdout,"ASF rc=%d codec=0x%04x ch=%u rate=%u bitrate=%u ms=%llu packet=%u data=%llu\n",rc,inf.codec_id,inf.channels,inf.sample_rate,inf.bitrate_bps,(unsigned long long)inf.duration_ms,inf.packet_size,(unsigned long long)inf.data_offset);
 CHECK(rc==0);CHECK(inf.codec_id==0x161 || inf.codec_id==0x160);CHECK(inf.channels==2);CHECK(inf.sample_rate==44100);
 CHECK(inf.packet_size>0);CHECK(inf.duration_ms>0);
 /* Invalid and truncated images must fail safely at any read chunk size. */
 for(i=1;i<=80;i++){
  ctx.short_max=(unsigned)i;
  CHECK(spwma_probe(io,&inf)==0);
 }
 CHECK(ctx.seek_count>100);
 ctx.sz=12;CHECK(spwma_probe(io,&inf)!=0);
 ctx.sz=0;CHECK(spwma_probe(io,&inf)!=0);
 fclose(ctx.fp);
 puts("PASS ASF WMA metadata and bounded short reads");return 0;
}
