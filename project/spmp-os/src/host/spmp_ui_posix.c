#define _POSIX_C_SOURCE 200809L
#include "spmp_ui_posix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
static int make_dirs(const char *full){
 char path[512];size_t n=strlen(full);
 if(n>=sizeof path)return -1;
 memcpy(path,full,n+1);
 for(size_t i=1;i<n;i++)if(path[i]=='/'){
  path[i]=0;
  if(mkdir(path,0700)&&errno!=EEXIST)return -1;
  path[i]='/';
 }
 return 0;
}
static int mkpath(spmp_ui_posix *fs,const char *name,char *dst,size_t cap){
 if(!fs||!name||strlen(name)>SPMP_UI_PATH_MAX)return -1;
 /* Only paths created by spmp_menu_save_path should be accepted. */
 if((strncmp(name,"saves/",6)&&strncmp(name,"settings/",9))||strstr(name,"..")||strchr(name,'\\'))return -1;
 int n=snprintf(dst,cap,"%s/%s",fs->root,name);
 return n<0||(size_t)n>=cap?-1:0;
}
static int save_file(void *ctx,const char *path,const uint8_t *src,size_t n){
 spmp_ui_posix *fs=(spmp_ui_posix*)ctx;
 if(!src||!n)return -1;
 char dst[512],temp[520];
 if(mkpath(fs,path,dst,sizeof dst)||make_dirs(dst))return -1;
 int t=snprintf(temp,sizeof temp,"%s.tmp",dst);
 if(t<0||(size_t)t>=sizeof temp)return -1;
 /* EXCL prevents following pre-existing temp-file symlinks. */
 if(unlink(temp)<0&&errno!=ENOENT)return -1; /* stale temp after power loss */
 int fd=open(temp,O_CREAT|O_EXCL|O_WRONLY,0600);
 if(fd<0)return -1;
 size_t pos=0;int good=1;
 while(pos<n){
  ssize_t k=write(fd,src+pos,n-pos);
  if(k<=0){good=0;break;}pos+=(size_t)k;
 }
 if(good&&fsync(fd))good=0;
 if(close(fd))good=0;
 if(good&&rename(temp,dst))good=0;
 if(!good){unlink(temp);return -1;}
 /* Data and directory metadata durable on typical POSIX filesystems. */
 char *slash=strrchr(dst,'/');
 if(slash){*slash=0;int dfd=open(dst,O_RDONLY|O_DIRECTORY);
   if(dfd>=0){(void)fsync(dfd);close(dfd);}}
 return 0;
}
static int load_file(void *ctx,const char *path,uint8_t *dst,size_t cap,size_t *actual){
 spmp_ui_posix *fs=(spmp_ui_posix*)ctx;
 char full[512];if(!dst||!actual||mkpath(fs,path,full,sizeof full))return -1;
 FILE *f=fopen(full,"rb");if(!f)return -1;
 size_t n=fread(dst,1,cap,f);
 int extra=fgetc(f),failed=ferror(f);
 fclose(f);
 if(extra!=EOF||failed)return -1;
 *actual=n;return 0;
}
int spmp_ui_posix_init(spmp_ui_posix *fs,const char *root){
 if(!fs||!root||!*root||strlen(root)>=sizeof fs->root)return -1;
 strcpy(fs->root,root);return 0;
}
static int prefix_file(void *ctx,const char *path,uint8_t *dst,size_t n){
 spmp_ui_posix *fs=(spmp_ui_posix*)ctx;
 char full[512];if(!dst||!n||mkpath(fs,path,full,sizeof full))return -1;
 FILE *f=fopen(full,"rb");if(!f)return -1;
 size_t got=fread(dst,1,n,f);
 int failed=ferror(f);fclose(f);
 return got==n&&!failed?0:-1;
}
spmp_ui_storage spmp_ui_posix_storage(spmp_ui_posix *fs){
 spmp_ui_storage r={fs,save_file,load_file,prefix_file};return r;
}
