/* SPDX-License-Identifier: MIT */
#include "spmp_file_copy.h"
enum { E_IO=5,E_BUSY=16,E_EXIST=17,E_INVAL=22,E_BIG=36,E_NOSYS=38,E_STALE=116,E_CANCEL=125 };
static unsigned length(const char *s){unsigned n=0;if(!s)return SPMP_COPY_PATH;while(n<SPMP_COPY_PATH&&s[n])n++;return n;}
static void copy(char *d,const char *s){while((*d++=*s++)){} }
static int equal(const char *a,const char *b){while(*a&&*a==*b){a++;b++;}return *a==*b;}
int spmp_file_copy_path_valid(const char *s){
 static const char root[]="SPMP-OS/User/";unsigned n=length(s),start=0;
 if(n>=SPMP_COPY_PATH||n<=sizeof root-1)return 0;
 for(unsigned i=0;i<sizeof root-1;i++)if(s[i]!=root[i])return 0;
 for(unsigned i=0;i<=n;i++){
  unsigned c=(uint8_t)s[i];
  if(!c||c=='/'){
   unsigned count=i-start;
   if(!count||count>255||(count==1&&s[start]=='.')||(count==2&&s[start]=='.'&&s[start+1]=='.')||s[i-1]=='.'||s[i-1]==' ')return 0;
   start=i+1;continue;
  }
  if(c<32||c==127||c=='\\'||c==':'||c=='*'||c=='?'||c=='"'||c=='<'||c=='>'||c=='|')return 0;
  if(c>=128){
   unsigned count,v,min;
   if(c>=0xc2&&c<=0xdf){count=1;v=c&31;min=0x80;}
   else if(c>=0xe0&&c<=0xef){count=2;v=c&15;min=0x800;}
   else if(c>=0xf0&&c<=0xf4){count=3;v=c&7;min=0x10000;}
   else return 0;
   if(i+count>=n)return 0;
   while(count--){c=(uint8_t)s[++i];if((c&0xc0)!=0x80)return 0;v=(v<<6)|(c&63);}
   if(v<min||v>0x10ffff||(v>=0xd800&&v<=0xdfff))return 0;
  }
 }
 return 1;
}
static int regular(const struct spmp_copy_meta *m){return (m->mode&9u)==8u;}
static int same(const struct spmp_copy_meta *a,const struct spmp_copy_meta *b){return regular(a)&&regular(b)&&a->bytes==b->bytes&&a->mtime==b->mtime;}
static int terminal(const struct spmp_file_copy *t){return t->phase==SPMP_COPY_IDLE||t->phase>=SPMP_COPY_DONE;}
int spmp_file_copy_init(struct spmp_file_copy *t,const struct spmp_copy_io *io){
 if(!t||!io||!io->stat||!io->fstat||!io->open_read||!io->create_exclusive||!io->read||!io->write||!io->seek_zero||!io->close||!io->publish_noreplace||!io->unlink_stage)return -E_INVAL;
 uint8_t *p=(uint8_t*)t;for(unsigned i=0;i<sizeof *t;i++)p[i]=0;
 t->io=*io;t->source_fd=t->writer_fd=t->verify_fd=-1;return 0;
}
int spmp_file_copy_start(struct spmp_file_copy *t,const char *src,const char *dst,uint32_t token){
 if(!t)return -E_INVAL;
 if(!terminal(t)||t->owned_stage)return -E_BUSY;
 if(!spmp_file_copy_path_valid(src)||!spmp_file_copy_path_valid(dst)||equal(src,dst))return -E_INVAL;
 unsigned parent=0;for(unsigned i=0;dst[i];i++)if(dst[i]=='/')parent=i+1;
 if(parent+sizeof ".spmp-copy-00000000.tmp">SPMP_COPY_PATH)return -E_BIG;
 copy(t->source,src);copy(t->destination,dst);t->stage[0]=0;t->token=token;
 t->written=t->verified=t->chunk=t->used=t->attempts=0;
 t->original=(struct spmp_copy_meta){0};
 t->error=t->cleanup_error=0;t->cancel_requested=t->published=0;t->phase=SPMP_COPY_CHECK;
 return 0;
}
static int close_fd(struct spmp_file_copy *t,int *fd){int value=*fd;*fd=-1;return value<0?0:t->io.close(t->io.user,value);}
static int fail(struct spmp_file_copy *t,int error){
 t->error=error<0?error:-E_IO;
 int *fds[]={&t->source_fd,&t->writer_fd,&t->verify_fd};
 for(unsigned i=0;i<3;i++){int r=close_fd(t,fds[i]);if(r&&!t->cleanup_error)t->cleanup_error=r<0?r:-E_IO;}
 if(t->owned_stage){int r=t->io.unlink_stage(t->io.user,t->stage);if(!r||r==-2)t->owned_stage=0;else if(!t->cleanup_error)t->cleanup_error=r<0?r:-E_IO;}
 t->phase=t->error==-E_CANCEL?SPMP_COPY_CANCELLED:SPMP_COPY_FAILED;return 1;
}
void spmp_file_copy_cancel(struct spmp_file_copy *t){if(t&&!terminal(t))t->cancel_requested=1;}
int spmp_file_copy_cleanup(struct spmp_file_copy *t){
 if(!t)return -E_INVAL;
 if(!terminal(t))return -E_BUSY;
 if(!t->owned_stage)return 0;
 int r=t->io.unlink_stage(t->io.user,t->stage);
 if(r&&r!=-2){t->cleanup_error=r<0?r:-E_IO;return t->cleanup_error;}
 t->owned_stage=0;t->cleanup_error=0;return 0;
}
static void stage_name(struct spmp_file_copy *t){
 unsigned n=0;for(unsigned i=0;t->destination[i];i++)if(t->destination[i]=='/')n=i+1;
 for(unsigned i=0;i<n;i++)t->stage[i]=t->destination[i];
 copy(t->stage+n,".spmp-copy-");n+=11;
 static const char hex[]="0123456789abcdef";uint32_t token=t->token+t->attempts;
 for(unsigned i=0;i<8;i++)t->stage[n++]=hex[(token>>(28-4*i))&15u];
 copy(t->stage+n,".tmp");
}
int spmp_file_copy_step(struct spmp_file_copy *t){
 if(!t)return 1;
 if(terminal(t))return 1;
 if(t->cancel_requested)return fail(t,-E_CANCEL);
 void *u=t->io.user;int r;struct spmp_copy_meta m;
 switch(t->phase){
 case SPMP_COPY_CHECK:
  r=t->io.stat(u,t->source,&t->original);if(r)return fail(t,r);
  if(!regular(&t->original))return fail(t,t->original.mode&1u?-21:-95);
  r=t->io.stat(u,t->destination,&m);if(!r)return fail(t,-E_EXIST);if(r!=-2)return fail(t,r);
  t->phase=SPMP_COPY_OPEN_SOURCE;break;
 case SPMP_COPY_OPEN_SOURCE:
  r=t->io.open_read(u,t->source);if(r<0)return fail(t,r);t->source_fd=r;
  r=t->io.fstat(u,r,&m);if(r)return fail(t,r);if(!same(&m,&t->original))return fail(t,-E_STALE);
  t->phase=SPMP_COPY_CREATE;break;
 case SPMP_COPY_CREATE:
  stage_name(t);r=t->io.create_exclusive(u,t->stage);
  if(r==-E_EXIST){if(++t->attempts==SPMP_COPY_STAGE_TRIES)return fail(t,r);break;}
  if(r<0)return fail(t,r);
  t->writer_fd=r;t->owned_stage=1;t->phase=SPMP_COPY_READ;break;
 case SPMP_COPY_READ:{
  uint32_t n=t->original.bytes-t->written;if(!n){t->phase=SPMP_COPY_SOURCE_EOF;break;}if(n>SPMP_COPY_BLOCK)n=SPMP_COPY_BLOCK;
  r=t->io.read(u,t->source_fd,t->block,n);if(r<=0)return fail(t,r<0?r:-E_STALE);if((uint32_t)r>n)return fail(t,-E_IO);
  t->chunk=(uint32_t)r;t->used=0;t->phase=SPMP_COPY_WRITE;break;
 }
 case SPMP_COPY_WRITE:
  r=t->io.write(u,t->writer_fd,t->block+t->used,t->chunk-t->used);
  if(r<=0||(uint32_t)r>t->chunk-t->used)return fail(t,r<0?r:-E_IO);
  t->used+=(uint32_t)r;t->written+=(uint32_t)r;if(t->used==t->chunk)t->phase=SPMP_COPY_READ;break;
 case SPMP_COPY_SOURCE_EOF:
  r=t->io.read(u,t->source_fd,t->block,1);if(r)return fail(t,r<0?r:-E_STALE);t->phase=SPMP_COPY_CLOSE_WRITER;break;
 case SPMP_COPY_CLOSE_WRITER:
  r=close_fd(t,&t->writer_fd);if(r)return fail(t,r);t->phase=SPMP_COPY_OPEN_VERIFY;break;
 case SPMP_COPY_OPEN_VERIFY:
  r=t->io.open_read(u,t->stage);if(r<0)return fail(t,r);t->verify_fd=r;
  r=t->io.fstat(u,r,&m);if(r)return fail(t,r);if(!regular(&m)||m.bytes!=t->original.bytes)return fail(t,-E_IO);
  t->phase=SPMP_COPY_REWIND;break;
 case SPMP_COPY_REWIND:
  r=t->io.seek_zero(u,t->source_fd);if(r)return fail(t,r);t->phase=SPMP_COPY_VERIFY_SOURCE;break;
 case SPMP_COPY_VERIFY_SOURCE:{
  uint32_t n=t->original.bytes-t->verified;if(!n){t->phase=SPMP_COPY_VERIFY_SOURCE_EOF;break;}if(n>SPMP_COPY_BLOCK)n=SPMP_COPY_BLOCK;
  r=t->io.read(u,t->source_fd,t->block,n);if(r<=0)return fail(t,r<0?r:-E_STALE);if((uint32_t)r>n)return fail(t,-E_IO);
  t->chunk=(uint32_t)r;t->used=0;t->phase=SPMP_COPY_VERIFY_TARGET;break;
 }
 case SPMP_COPY_VERIFY_TARGET:
  r=t->io.read(u,t->verify_fd,t->verify+t->used,t->chunk-t->used);
  if(r<=0||(uint32_t)r>t->chunk-t->used)return fail(t,r<0?r:-E_IO);
  t->used+=(uint32_t)r;
  if(t->used==t->chunk){for(unsigned i=0;i<t->chunk;i++)if(t->block[i]!=t->verify[i])return fail(t,-E_IO);t->verified+=t->chunk;t->phase=SPMP_COPY_VERIFY_SOURCE;}
  break;
 case SPMP_COPY_VERIFY_SOURCE_EOF:
  r=t->io.read(u,t->source_fd,t->block,1);if(r)return fail(t,r<0?r:-E_STALE);t->phase=SPMP_COPY_VERIFY_TARGET_EOF;break;
 case SPMP_COPY_VERIFY_TARGET_EOF:
  r=t->io.read(u,t->verify_fd,t->verify,1);if(r)return fail(t,r<0?r:-E_IO);t->phase=SPMP_COPY_RECHECK;break;
 case SPMP_COPY_RECHECK:
  r=t->io.fstat(u,t->source_fd,&m);if(r)return fail(t,r);if(!same(&m,&t->original))return fail(t,-E_STALE);
  r=t->io.stat(u,t->source,&m);if(r)return fail(t,r);if(!same(&m,&t->original))return fail(t,-E_STALE);
  t->phase=SPMP_COPY_CLOSE_READERS;break;
 case SPMP_COPY_CLOSE_READERS:
  r=close_fd(t,&t->source_fd);if(r)return fail(t,r);r=close_fd(t,&t->verify_fd);if(r)return fail(t,r);
  t->phase=SPMP_COPY_PUBLISH;break;
 case SPMP_COPY_PUBLISH:
  r=t->io.publish_noreplace(u,t->stage,t->destination);if(r)return fail(t,r);
  t->owned_stage=0;t->published=1;t->phase=SPMP_COPY_DONE;return 1;
 default:return fail(t,-E_NOSYS);
 }
 return 0;
}
