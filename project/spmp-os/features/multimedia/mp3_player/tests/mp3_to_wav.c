/* Host-only test wrapper; the ARM library has no stdio dependency. */
#define _FILE_OFFSET_BITS 64
#include "spmp_mp3_player.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>

typedef struct {FILE *in,*out;uint64_t size;uint64_t data_bytes;unsigned hz,ch;int failed;size_t read_chunk;} host;
static int read_cb(void *ctx,uint8_t *out,size_t size){host *h=ctx;if(h->read_chunk && size>h->read_chunk)size=h->read_chunk;size_t z=fread(out,1,size,h->in);if(!z && ferror(h->in))return -1;return (int)z;}
static int seek_cb(void *ctx,uint64_t offset){host *h=ctx;if(offset>0x7fffffffffffffffULL)return -1;return fseeko(h->in,(off_t)offset,SEEK_SET)==0?0:-1;}
static uint64_t size_cb(void *ctx){return ((host*)ctx)->size;}
static void le16(FILE *f,unsigned n){fputc(n&255,f);fputc((n>>8)&255,f);}
static void le32(FILE *f,uint32_t n){le16(f,n&65535);le16(f,n>>16);}
static void wav_header(host *h){
 uint64_t bytes=h->data_bytes;uint32_t n=bytes>0xffffffffu-36?0xffffffffu-36:(uint32_t)bytes;
 if(fseeko(h->out,0,SEEK_SET)!=0){h->failed=1;return;}
 fwrite("RIFF",1,4,h->out);le32(h->out,36+n);fwrite("WAVEfmt ",1,8,h->out);
 le32(h->out,16);le16(h->out,1);le16(h->out,h->ch);le32(h->out,h->hz);
 le32(h->out,h->hz*h->ch*2);le16(h->out,h->ch*2);le16(h->out,16);
 fwrite("data",1,4,h->out);le32(h->out,n);
}
static int pcm_cb(void *ctx,const int16_t *samples,size_t frames,unsigned hz,unsigned ch){
 host *h=ctx;size_t i,count=frames*ch;
 if(!h->hz){h->hz=hz;h->ch=ch;}
 if(h->hz!=hz||h->ch!=ch)return -1;
 for(i=0;i<count;i++){uint16_t v=(uint16_t)samples[i];if(fputc(v&255,h->out)==EOF||fputc(v>>8,h->out)==EOF)return -1;}
 h->data_bytes+=count*2;return 0;
}
int main(int argc,char **argv){
 host h={0};spmp_mp3_player *p;spmp_mp3_io io;const spmp_mp3_metadata *m;int rc,steps=0,nonseek=0;uint32_t seek_ms=0;
 if(argc<3 || argc>6){fprintf(stderr,"Usage: mp3_to_wav in.mp3 out.wav [noseek|seek=milliseconds] [noseek]\n");return 2;}
 for(int i=3;i<argc;i++){if(!strcmp(argv[i],"noseek"))nonseek=1;else if(!strncmp(argv[i],"chunk=",6))h.read_chunk=strtoul(argv[i]+6,NULL,10);else if(!strncmp(argv[i],"seek=",5))seek_ms=(uint32_t)strtoul(argv[i]+5,NULL,10);else return 2;}
 if(!(h.in=fopen(argv[1],"rb")))return 3;
 if(fseeko(h.in,0,SEEK_END)!=0){fclose(h.in);return 3;}
 h.size=(uint64_t)ftello(h.in);fseeko(h.in,0,SEEK_SET);
 if(!(h.out=fopen(argv[2],"wb+"))){fclose(h.in);return 3;}
 for(int i=0;i<44;i++)fputc(0,h.out);
 p=calloc(1,spmp_mp3_player_size());if(!p)return 4;
 io.ctx=&h;io.read=read_cb;io.seek=nonseek?NULL:seek_cb;io.size=nonseek?NULL:size_cb;
 rc=spmp_mp3_open(p,&io,pcm_cb,&h);if(rc<0){fprintf(stderr,"open failed\n");return 5;}
 /* Prime first frame: seek relies on bitrate/duration headers. */
 if(seek_ms){rc=spmp_mp3_step(p);if(rc!=1 || spmp_mp3_seek_ms(p,seek_ms)!=0){fprintf(stderr,"seek failed\n");return 6;}
   /* Reset initial test WAV output to contain only post-seek PCM. */
   h.data_bytes=0;fseeko(h.out,44,SEEK_SET);
 }
 while((rc=spmp_mp3_step(p))==1){if(++steps>500000)return 7;}
 m=spmp_mp3_info(p);
 fprintf(stderr,"MP3: status=%d frames=%d bytes=%" PRIu64 " rate=%u channels=%u bitrate=%u duration=%u bad=%u title=%s artist=%s cover=%s coverbytes=%u\n",rc,steps,h.data_bytes,m->sample_rate,m->channels,m->bitrate_kbps,m->duration_ms,spmp_mp3_bad_frames(p),m->title,m->artist,m->cover_mime,m->cover_bytes);
 if(rc!=0||!steps||!h.data_bytes||!h.hz||h.failed){return 8;}
 wav_header(&h);spmp_mp3_close(p);free(p);fclose(h.out);fclose(h.in);return 0;
}
