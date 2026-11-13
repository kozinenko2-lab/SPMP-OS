#include "spmp_filestream.h"
#include <streams/file_stream.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

typedef struct { int64_t at; int opened; } test_file;
typedef struct { test_file f[8]; unsigned index; unsigned closes; } test_hal;
static const unsigned char data[]="GBA-CARTRIDGE-CONTENT";
static void *op(void *ctx,const char *p) {
  test_hal *h=(test_hal*)ctx;
  if(strcmp(p,"game.gba") || h->index>=8) return 0;
  test_file *f=&h->f[h->index++]; f->at=0;f->opened=1;return f;
}
static int64_t sz(void *ctx,void *ptr) { (void)ctx; return ((test_file*)ptr)->opened ? (int64_t)(sizeof(data)-1) : -1; }
static int64_t seekf(void *ctx,void *ptr,int64_t off,int whence) {
  test_file *f=ptr; int64_t at=whence==0?0:(whence==1?f->at:(int64_t)(sizeof(data)-1)); (void)ctx;
  if (off > (int64_t)sizeof(data) || off < -(int64_t)sizeof(data) || at+off<0 || at+off>(int64_t)(sizeof(data)-1)) return -1;
  return f->at=at+off;
}
static int64_t readf(void *ctx,void *ptr,void *dst,int64_t n) {
  test_file *f=ptr;int64_t available=(int64_t)(sizeof(data)-1)-f->at; (void)ctx;
  if(n>available)n=available;
  memcpy(dst,data+f->at,(size_t)n);f->at+=n;return n;
}
static int closef(void *ctx,void *ptr){test_hal *h=ctx;((test_file*)ptr)->opened=0;h->closes++;return 0;}
int main(void) {
  test_hal h={0};spmp_file_hal io={&h,op,sz,seekf,readf,closef};char b[32];RFILE *f[4];
  assert(!filestream_open("game.gba",RETRO_VFS_FILE_ACCESS_READ,0));
  assert(spmp_file_bind(0)<0);
  assert(spmp_file_bind(&io)==0);
  for(int i=0;i<4;i++) { f[i]=filestream_open("game.gba",RETRO_VFS_FILE_ACCESS_READ,0);assert(f[i]);}
  assert(spmp_file_open_count()==4);
  assert(!filestream_open("game.gba",RETRO_VFS_FILE_ACCESS_READ,0));
  assert(!filestream_open("other",RETRO_VFS_FILE_ACCESS_READ,0));
  assert(!filestream_open("game.gba",RETRO_VFS_FILE_ACCESS_WRITE,0));
  assert(spmp_file_unbind()<0);
  assert(filestream_get_size(f[0])==(int64_t)(sizeof(data)-1));
  assert(filestream_read(f[0],b,3)==3 && !memcmp(b,"GBA",3));
  assert(filestream_seek(f[0],0,0)==0);
  assert(filestream_read(f[0],b,32)==(int64_t)(sizeof(data)-1));
  assert(!memcmp(b,data,sizeof(data)-1));
  assert(filestream_read(f[0],b,1)==0);
  assert(filestream_read(f[0],b,-1)==-1);
  assert(filestream_seek(f[0],-1,0)==-1);
  assert(filestream_read((RFILE*)(uintptr_t)1,b,1)==-1);
  for(int i=0;i<4;i++)assert(filestream_close(f[i])==0);
  assert(filestream_close(f[0])==-1);
  assert(spmp_file_open_count()==0 && h.closes==4);
  assert(spmp_file_unbind()==0);
  assert(spmp_file_bind(&io)==0);
  assert(filestream_open("game.gba",RETRO_VFS_FILE_ACCESS_READ,0));
  assert(spmp_file_open_count()==1);
  /* handles must be closed by the core before RAM unload */
  assert(spmp_file_unbind()<0);
  assert(filestream_close(f[0])==0);
  assert(spmp_file_unbind()==0);
  puts("gpSP filestream HAL: PASS");return 0;
}
