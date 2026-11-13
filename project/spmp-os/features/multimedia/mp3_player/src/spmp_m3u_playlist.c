#include "spmp_m3u_playlist.h"
#include <string.h>
void spmp_m3u_open(spmp_m3u_reader *p,const spmp_mp3_io *io){
 memset(p,0,sizeof(*p));if(io)p->io=*io;
}
static int next_char(spmp_m3u_reader *p){
 int n;
 if(p->at==p->end){
  if(p->eof)return -1;
  n=p->io.read(p->io.ctx,p->block,sizeof(p->block));
  if(n<0 || n>(int)sizeof(p->block))return -2;
  if(n==0){p->eof=1;return -1;}
  p->at=0;p->end=(size_t)n;
 }
 return p->block[p->at++];
}
int spmp_m3u_next(spmp_m3u_reader *p,char out[SPMP_M3U_MAX_PATH]){
 unsigned n;int x;size_t i;
 if(!p||!p->io.read||!out)return -1;
 for(;;){
  n=0;
  for(;;){
   x=next_char(p);if(x==-2)return -1;
   if(x==-1 || x=='\n')break;
   if(x!='\r'){
    if(n>=SPMP_M3U_MAX_PATH-1)return -1;
    out[n++]=(char)x;
   }
  }
  if(x==-1 && !n)return 0;
  p->line_no++;
  out[n]=0;
  if(p->line_no==1 && n>=3 && (unsigned char)out[0]==0xef && (unsigned char)out[1]==0xbb && (unsigned char)out[2]==0xbf){
   memmove(out,out+3,n-2);n-=3;
  }
  for(i=0;i<n && (out[i]==' '||out[i]=='\t');i++);
  if(i){memmove(out,out+i,n-i+1);n-=(unsigned)i;}
  while(n&&(out[n-1]==' '||out[n-1]=='\t'))out[--n]=0;
  if(n && out[0]!='#')return 1;
  if(x==-1)return 0;
 }
}
