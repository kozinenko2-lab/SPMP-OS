/* Bounded, allocation-free ID3v1/ID3v2 metadata, GPL-2.0-or-later. */
#include "spmp_mp3_player.h"
#include <string.h>

static int exact(const spmp_mp3_io *io, uint8_t *dest, size_t n) {
 size_t got=0;int z;
 while(got<n) {z=io->read(io->ctx,dest+got,n-got);if(z<=0 || (size_t)z>n-got)return -1;got+=(size_t)z;}
 return 0;
}
static uint32_t be32(const uint8_t *v) {
 return ((uint32_t)v[0]<<24)|((uint32_t)v[1]<<16)|((uint32_t)v[2]<<8)|v[3];
}
static uint32_t synchsafe(const uint8_t *v) {
 return ((uint32_t)(v[0]&127)<<21)|((uint32_t)(v[1]&127)<<14)|((uint32_t)(v[2]&127)<<7)|(v[3]&127);
}
static void append_utf8(char *out,size_t cap,size_t *w,unsigned code) {
 if(code==0 || code==0xfeff)return;
 if(code<0x80){if(*w+1<cap)out[(*w)++]=(char)code;}
 else if(code<0x800){if(*w+2<cap){out[(*w)++]=(char)(0xc0|(code>>6));out[(*w)++]=(char)(0x80|(code&63));}}
 else if(code<0x10000){if(*w+3<cap){out[(*w)++]=(char)(0xe0|(code>>12));out[(*w)++]=(char)(0x80|((code>>6)&63));out[(*w)++]=(char)(0x80|(code&63));}}
 else if(code<=0x10ffff){if(*w+4<cap){out[(*w)++]=(char)(0xf0|(code>>18));out[(*w)++]=(char)(0x80|((code>>12)&63));out[(*w)++]=(char)(0x80|((code>>6)&63));out[(*w)++]=(char)(0x80|(code&63));}}
}
static void decode_text(char *out, size_t cap, const uint8_t *data,size_t bytes) {
 unsigned enc;size_t w=0,i=1;int le=1;unsigned x;
 if(!cap)return;out[0]=0;if(!bytes)return;enc=data[0];
 if(enc==1 || enc==2) {
  if(enc==1 && bytes>=3){if(data[1]==0xfe && data[2]==0xff)le=0;else if(data[1]==0xff&&data[2]==0xfe)le=1;else i=1;if(i==1 || data[1]==0xfe || data[1]==0xff)i=3;}
  if(enc==2)le=0;
  while(i+1<bytes){unsigned y; x=le?(data[i]|((unsigned)data[i+1]<<8)):(((unsigned)data[i]<<8)|data[i+1]);i+=2;if(!x)break;
   if(x>=0xd800 && x<=0xdbff && i+1<bytes){y=le?(data[i]|((unsigned)data[i+1]<<8)):(((unsigned)data[i]<<8)|data[i+1]);if(y>=0xdc00&&y<=0xdfff){x=0x10000+((x-0xd800)<<10)+(y-0xdc00);i+=2;}}
   if(x>=0xd800&&x<=0xdfff)x='?';append_utf8(out,cap,&w,x);
  }
 } else if(enc==3) {
  while(i<bytes && data[i]){if(w+1<cap)out[w++]=(char)data[i];i++;}
 } else {
  while(i<bytes && data[i]){append_utf8(out,cap,&w,data[i]);i++;}
 }
 out[w]=0;
}
static void id3v1_value(char *out,size_t cap,const uint8_t *v,size_t n){
 size_t i=0,w=0;while(n && (v[n-1]==' ' || v[n-1]==0))n--;if(out[0] || !cap)return;
 for(i=0;i<n;i++)append_utf8(out,cap,&w,v[i]);out[w]=0;
}
static void image_info(spmp_mp3_metadata *out,const spmp_mp3_io *io, uint64_t pos,uint32_t len,int v){
 uint8_t prefix[160];size_t n=(len<sizeof(prefix)?len:sizeof(prefix)),idx=0,m=0;
 const char *mime=0;
 if(n<8 || io->seek(io->ctx,pos)!=0 || exact(io,prefix,n)!=0)return;
 idx=1; /* text encoding */
 if(v==2){if(n<8)return;idx=5;mime=(prefix[1]=='P'&&prefix[2]=='N'&&prefix[3]=='G')?"image/png":"image/jpeg";}
 else {
  size_t start=idx;while(idx<n && prefix[idx])idx++;
  if(idx>=n)return;
  if(idx-start==10 && !memcmp(prefix+start,"image/jpeg",10))mime="image/jpeg";
  else if(idx-start==9 && !memcmp(prefix+start,"image/png",9))mime="image/png";
  else return;
  idx++;
 }
 if(idx>=n)return;idx++; /* picture type */
 if(prefix[0]==1 || prefix[0]==2){
  while(idx+1<n && (prefix[idx]||prefix[idx+1]))idx+=2;idx+=2;
 }else{while(idx<n && prefix[idx])idx++;idx++;}
 if(idx+8>n || idx>=len)return;
 if(mime && ((!memcmp(prefix+idx,"\xff\xd8\xff",3) && !memcmp(mime,"image/jpeg",11)) ||
               (!memcmp(prefix+idx,"\x89PNG\r\n\x1a\n",8) && !memcmp(mime,"image/png",10)))){
  out->cover_offset=pos+idx;out->cover_bytes=len-(uint32_t)idx;
  while(mime[m]&&m+1<sizeof(out->cover_mime)){out->cover_mime[m]=mime[m];m++;}
  out->cover_mime[m]=0;
 }
}
static int tag_header(const uint8_t h[10],uint32_t *bytes,int *version,int *flags){
 if(memcmp(h,"ID3",3)!=0)return 0;
 if(h[3]<2 || h[3]>4 || h[4]!=0 || (h[6]|h[7]|h[8]|h[9])&0x80)return -1;
 *version=h[3];*flags=h[5];*bytes=synchsafe(h+6);
 if(*bytes>16u*1024u*1024u)return -1;
 return 1;
}
int spmp_mp3_read_metadata(const spmp_mp3_io *io,spmp_mp3_metadata *out) {
 uint8_t h[10], fh[10], body[2048], v1[128];uint32_t bytes=0;int version=0,flags=0,tag,nd;
 uint64_t pos=10,limit=10;size_t hdrlen;
 if(!io||!out||!io->read||!io->seek)return -1;
 memset(out,0,sizeof(*out));
 if(io->seek(io->ctx,0)!=0 || exact(io,h,10)!=0)return -1;
 tag=tag_header(h,&bytes,&version,&flags);
 if(tag<0)return -1;
 if(tag==1){
  limit=10+(uint64_t)bytes;out->id3v2_bytes=(uint32_t)(limit+((flags&0x10)?10:0));out->audio_offset=out->id3v2_bytes;
  hdrlen=version==2?6:10;
  /* Extended headers have different sizes across v2.3/v2.4: skip conservatively. */
  if(flags&0x40){uint8_t eh[4];if(version==2)goto v1fallback;if(io->seek(io->ctx,pos)!=0||exact(io,eh,4))goto v1fallback;
   pos+= (version==4?synchsafe(eh):be32(eh)+(uint32_t)4);if(pos>limit)goto v1fallback;
  }
  while(pos+hdrlen<=limit){uint32_t len;char *dst=0;size_t cap=0,take;uint16_t ff=0;
   if(io->seek(io->ctx,pos)!=0 || exact(io,fh,hdrlen)!=0)break;
   if(fh[0]==0)break;
   len=version==2?(((uint32_t)fh[3]<<16)|((uint32_t)fh[4]<<8)|fh[5]):(version==4?synchsafe(fh+4):be32(fh+4));
   if(version!=2)ff=((uint16_t)fh[8]<<8)|fh[9];
   pos+=hdrlen;if(len>limit-pos)break;
   if((version==2 && !memcmp(fh,"TT2",3))||(version!=2&&!memcmp(fh,"TIT2",4))){dst=out->title;cap=sizeof(out->title);}
   else if((version==2&&!memcmp(fh,"TP1",3))||(version!=2&&!memcmp(fh,"TPE1",4))){dst=out->artist;cap=sizeof(out->artist);}
   else if((version==2&&!memcmp(fh,"TAL",3))||(version!=2&&!memcmp(fh,"TALB",4))){dst=out->album;cap=sizeof(out->album);}
   else if((version==2&&!memcmp(fh,"TCO",3))||(version!=2&&!memcmp(fh,"TCON",4))){dst=out->genre;cap=sizeof(out->genre);}
   else if((version==2&&!memcmp(fh,"TRK",3))||(version!=2&&!memcmp(fh,"TRCK",4))){dst=out->track;cap=sizeof(out->track);}
   else if((version==2&&!memcmp(fh,"TYE",3))||(version!=2&&(!memcmp(fh,"TYER",4)||!memcmp(fh,"TDRC",4)))){dst=out->year;cap=sizeof(out->year);}
   /* Reject compressed/encrypted/grouped or unsynchronised frames until explicitly supported. */
   if(!dst && !out->cover_bytes && len && !(flags&0x80) && !(ff & (version==4?0x004f:0x00e0)) &&
     ((version==2&&!memcmp(fh,"PIC",3))||(version!=2&&!memcmp(fh,"APIC",4))))
    image_info(out,io,pos,len,version);
   if(dst && len && !(flags&0x80) && !(ff & (version==4?0x004f:0x00e0))){
    take=len<sizeof(body)?len:sizeof(body);if(io->seek(io->ctx,pos)==0 && exact(io,body,take)==0)decode_text(dst,cap,body,take);
   }
   pos+=len;
  }
 }else out->audio_offset=0;
v1fallback:
 if(io->size){uint64_t total=io->size(io->ctx);
  if(total>=128 && io->seek(io->ctx,total-128)==0 && exact(io,v1,128)==0 && !memcmp(v1,"TAG",3)) {
   id3v1_value(out->title,sizeof(out->title),v1+3,30);id3v1_value(out->artist,sizeof(out->artist),v1+33,30);
   id3v1_value(out->album,sizeof(out->album),v1+63,30);id3v1_value(out->year,sizeof(out->year),v1+93,4);
   if(v1[125]==0 && v1[126] && !out->track[0]){unsigned t=v1[126];if(t>=100){out->track[0]=(char)('0'+t/100);out->track[1]=(char)('0'+(t/10)%10);out->track[2]=(char)('0'+t%10);out->track[3]=0;}else if(t>=10){out->track[0]=(char)('0'+t/10);out->track[1]=(char)('0'+t%10);out->track[2]=0;}else{out->track[0]=(char)('0'+t);out->track[1]=0;}}
  }
 }
 return 0;
}
