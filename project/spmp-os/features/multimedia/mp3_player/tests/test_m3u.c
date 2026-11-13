#include "spmp_m3u_playlist.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
typedef struct {const char*data;size_t offset,len;} input;
static int rd(void *ctx,uint8_t *buf,size_t limit){input *p=ctx;size_t n=p->len-p->offset;if(n>limit)n=limit;if(n>3)n=3;memcpy(buf,p->data+p->offset,n);p->offset+=n;return (int)n;}
int main(void){const char s[]="\xef\xbb\xbf#EXTM3U\r\n#EXTINF:20,Hello\r\ntrack1.mp3\r\n  music/Трек 02.mp3  \n\n";
 input i={s,0,sizeof(s)-1};spmp_mp3_io io={0};spmp_m3u_reader r;char out[SPMP_M3U_MAX_PATH];
 io.ctx=&i;io.read=rd;spmp_m3u_open(&r,&io);
 assert(spmp_m3u_next(&r,out)==1 && !strcmp(out,"track1.mp3"));
 assert(spmp_m3u_next(&r,out)==1 && !strcmp(out,"music/Трек 02.mp3"));
 assert(spmp_m3u_next(&r,out)==0);
 puts("M3U UTF8 PASS");return 0;}
