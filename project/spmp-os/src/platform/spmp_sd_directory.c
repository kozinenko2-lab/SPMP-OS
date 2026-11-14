#include "spmp_sd_directory.h"
static size_t length(const char *s,size_t cap){size_t n=0;while(n<cap&&s[n])n++;return n;}
static void copy(void *to,const void *from,size_t n){uint8_t *d=to;const uint8_t *s=from;while(n--)*d++=*s++;}
static int equal(const char *a,const char *b){while(*a&&*a==*b){a++;b++;}return *a==*b;}
static int segment(const char *s,size_t n){
 if(!n||(n==1&&s[0]=='.')||(n==2&&s[0]=='.'&&s[1]=='.'))return 0;
 for(size_t i=0;i<n;i++)if((uint8_t)s[i]<32||s[i]=='/'||s[i]=='\\'||s[i]==':')return 0;
 return 1;
}
int spmp_sd_path(const struct spmp_sd_directory *d,const char *rel,char *out,size_t cap){
 if(!d||!rel||!out||rel[0]=='/')return -1;
 size_t a=length(d->root,sizeof d->root),b=length(rel,FM01_PATH_MAX);
 if(b==FM01_PATH_MAX||a+b+(b?1u:0u)>=cap)return -1;
 size_t start=0;
 for(size_t i=0;i<=b;i++)if(i==b||rel[i]=='/'){
  if((b||i)&&!segment(rel+start,i-start))return -1;
  start=i+1;
 }
 copy(out,d->root,a);if(b)out[a++]='/';copy(out+a,rel,b+1);return 0;
}
int spmp_sd_directory_init(struct spmp_sd_directory *d,const struct spmp_sd_io *io,const char *root){
 if(!d||!io||!io->open||!io->next||!io->stat||!io->close||!root||root[0]!='/')return -1;
 size_t n=length(root,sizeof d->root);if(n==0||n>=sizeof d->root||root[n-1]=='/')return -1;
 uint8_t *p=(uint8_t*)d;for(size_t i=0;i<sizeof *d;i++)p[i]=0;
 d->io.user=io->user;d->io.open=io->open;d->io.next=io->next;d->io.stat=io->stat;d->io.close=io->close;
 copy(d->root,root,n+1);return 0;
}
void spmp_sd_directory_invalidate(struct spmp_sd_directory *d){if(d)d->valid=0;}
static int order(const fm01_entry *a,const fm01_entry *b){
 if(a->type!=b->type)return a->type==FM01_ENTRY_DIR?-1:1;
 const uint8_t *x=(const uint8_t*)a->name,*y=(const uint8_t*)b->name;
 while(*x&&*x==*y){x++;y++;}return (int)*x-(int)*y;
}
static int scan(struct spmp_sd_directory *d,const char *path,uint32_t offset,int has_floor){
 if(spmp_sd_path(d,path,d->absolute,sizeof d->absolute))return -1;
 int fd=d->io.open(d->io.user,d->absolute);if(fd<=0)return fd<0?fd:-5;
 d->count=d->total=d->files=d->dirs=0;int rc=0;uint32_t records=0;
 for(;;){
  int next=d->io.next(d->io.user,fd,d->name);if(next<=0){rc=next;break;}
  if(++records>SPMP_SD_SCAN_MAX){rc=-75;break;}
  size_t n=length(d->name,sizeof d->name);
  if((n==1&&d->name[0]=='.')||(n==2&&d->name[0]=='.'&&d->name[1]=='.'))continue;
  if(n>=FM01_NAME_MAX||!segment(d->name,n)){rc=-84;break;}
  if(spmp_sd_path(d,path,d->absolute,sizeof d->absolute)){rc=-1;break;}
  size_t a=length(d->absolute,sizeof d->absolute);
  if(a+n+1>=sizeof d->absolute){rc=-36;break;}
  d->absolute[a++]='/';copy(d->absolute+a,d->name,n+1);
  struct spmp_sd_meta meta;rc=d->io.stat(d->io.user,d->absolute,&meta);if(rc)break;
  if(!(meta.mode&9u))continue;
  fm01_entry *e=&d->candidate;uint8_t *zero=(uint8_t*)e;for(size_t i=0;i<sizeof *e;i++)zero[i]=0;
  copy(e->name,d->name,n+1);e->type=(meta.mode&1)?FM01_ENTRY_DIR:FM01_ENTRY_FILE;e->size_bytes=meta.bytes;e->read_only=1;
  d->total++;if(e->type==FM01_ENTRY_DIR)d->dirs++;else d->files++;
  if(has_floor&&order(e,&d->floor)<=0)continue;
  unsigned pos=0;while(pos<d->count&&order(&d->page[pos],e)<0)pos++;
  if(pos==FM01_PAGE_ROWS)continue;
  unsigned end=d->count<FM01_PAGE_ROWS?d->count:FM01_PAGE_ROWS-1;
  while(end>pos){copy(&d->page[end],&d->page[end-1],sizeof *e);end--;}
  copy(&d->page[pos],e,sizeof *e);if(d->count<FM01_PAGE_ROWS)d->count++;
 }
 int close=d->io.close(d->io.user,fd);d->scans++;if(!rc&&close)rc=close;
 if(rc){d->valid=0;return rc;}
 copy(d->path,path,length(path,FM01_PATH_MAX)+1);d->offset=offset;d->valid=1;return 0;
}
int spmp_sd_list_page(void *ctx,const char *path,uint32_t offset,uint32_t cap,fm01_entry *out,uint32_t *returned,uint32_t *total,uint32_t *files,uint32_t *dirs){
 struct spmp_sd_directory *d=ctx;
 if(!d||!path||!out||!returned||!total||!files||!dirs||cap>FM01_PAGE_ROWS||offset%FM01_PAGE_ROWS||offset>SPMP_SD_SCAN_MAX)return -1;
 if(!d->valid||!equal(path,d->path)){int r=scan(d,path,0,0);if(r)return r;}
 if(cap&&offset!=d->offset){
  if(offset<d->offset){int r=scan(d,path,0,0);if(r)return r;}
  while(offset>d->offset){
   if(d->count!=FM01_PAGE_ROWS)return -1;
   copy(&d->floor,&d->page[d->count-1],sizeof d->floor);
   uint32_t next=d->offset+FM01_PAGE_ROWS;int r=scan(d,path,next,1);if(r)return r;
  }
 }
 *returned=cap<d->count?cap:d->count;*total=d->total;*files=d->files;*dirs=d->dirs;
 for(uint32_t i=0;i<*returned;i++)copy(out+i,d->page+i,sizeof *out);
 return 0;
}
