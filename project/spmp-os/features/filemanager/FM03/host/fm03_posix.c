/* SPDX-License-Identifier: MIT; host-only secure POSIX adapter. */
#define _POSIX_C_SOURCE 200809L
#include "fm03_posix.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>
#ifndef O_NOFOLLOW
#error O_NOFOLLOW required
#endif
static int valid_name(const char *p){
    unsigned n=0;if(!p)return 0;
    while(p[n] && n<FM01_NAME_MAX){unsigned char c=(unsigned char)p[n];
        if(c=='/'||c=='\\'||c<32||c==127)return 0;
        n++;
    }
    return n>0&&n<FM01_NAME_MAX&&strcmp(p,".")&&strcmp(p,"..");
}
static int verify_directory(fm03_posix *fs,int fd){struct stat st;
    if(fstat(fd,&st)<0)return -errno;
    if(!S_ISDIR(st.st_mode)||st.st_dev!=fs->device)return -EXDEV;
    return 0;
}
static int open_parent(fm03_posix *fs,const char *relative){
    size_t i=0;int fd,rc;
    if(!relative||relative[0]=='/'||strlen(relative)>=FM01_PATH_MAX)return -EINVAL;
    fd=dup(fs->rootfd);if(fd<0)return -errno;
    while(relative[i]){
        char part[FM01_NAME_MAX];unsigned j=0;int next;
        while(relative[i] && relative[i]!='/'){
            if(j+1>=sizeof(part)){close(fd);return -ENAMETOOLONG;}
            part[j++]=relative[i++];
        }
        part[j]='\0';
        if(!valid_name(part) || (relative[i]=='/' && !relative[i+1])){close(fd);return -EINVAL;}
        if(relative[i]=='/')i++;
        next=openat(fd,part,O_DIRECTORY|O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
        if(next<0){rc=-errno;close(fd);return rc;}
        close(fd);fd=next;
        rc=verify_directory(fs,fd);if(rc){close(fd);return rc;}
    }
    return fd;
}
static int lookup(fm03_posix *fs,const char *path,const char *name,struct stat *st){
    int fd,rc;
    if(!valid_name(name))return -EINVAL;
    fd=open_parent(fs,path);if(fd<0)return fd;
    if(fstatat(fd,name,st,AT_SYMLINK_NOFOLLOW)<0)rc=-errno;
    else if(st->st_dev!=fs->device)rc=-EXDEV;
    else if(!S_ISREG(st->st_mode)&&!S_ISDIR(st->st_mode))rc=-EOPNOTSUPP;
    else rc=0;
    close(fd);return rc;
}
int fm03_posix_init(fm03_posix *fs,const char *root){
    struct stat st;
    if(!fs||!root)return -EINVAL;
    memset(fs,0,sizeof(*fs));fs->rootfd=-1;
    fs->rootfd=open(root,O_DIRECTORY|O_RDONLY|O_NOFOLLOW|O_CLOEXEC);
    if(fs->rootfd<0)return -errno;
    if(fstat(fs->rootfd,&st)<0){int err=-errno;close(fs->rootfd);fs->rootfd=-1;return err;}
    fs->device=st.st_dev;return 0;
}
static void end_scan(void *ctx){
    fm03_posix *fs=(fm03_posix*)ctx;
    if(!fs)return;
    while(fs->depth){unsigned i=--fs->depth;if(fs->frames[i])closedir(fs->frames[i]);fs->frames[i]=NULL;}
}
void fm03_posix_close(fm03_posix *fs){
    if(!fs)return;
    end_scan(fs);
    if(fs->rootfd>=0){close(fs->rootfd);fs->rootfd=-1;}
}
static int get_info(void *ctx,const char *parent,const char *name,fm03_info *info){
    fm03_posix *fs=(fm03_posix*)ctx;
    struct stat st;struct tm tmv;
    size_t p,n;int rc;
    if(!fs||fs->rootfd<0||!info)return -EINVAL;
    rc=lookup(fs,parent,name,&st);if(rc)return rc;
    p=strlen(parent);n=strlen(name);
    if(p+n+(p?1u:0u)+1u>FM01_PATH_MAX)return -ENAMETOOLONG;
    memset(info,0,sizeof(*info));
    memcpy(info->name,name,n+1);
    if(p){memcpy(info->path,parent,p);info->path[p++]='/';}
    memcpy(info->path+p,name,n+1);
    info->kind=S_ISDIR(st.st_mode)?FM01_ENTRY_DIR:FM01_ENTRY_FILE;
    if(S_ISREG(st.st_mode)){
        if(st.st_size<0)return -EOVERFLOW;
        info->bytes=(uint64_t)st.st_size;
    }
    if(!(st.st_mode&S_IWUSR))info->attrs|=FM03_ATTR_READ_ONLY;
    if(name[0]=='.')info->attrs|=FM03_ATTR_HIDDEN;
    if(localtime_r(&st.st_mtime,&tmv)){
        info->year=(uint16_t)(tmv.tm_year+1900);
        info->month=(uint8_t)(tmv.tm_mon+1);
        info->day=(uint8_t)tmv.tm_mday;
        info->hour=(uint8_t)tmv.tm_hour;
        info->minute=(uint8_t)tmv.tm_min;
        info->second=(uint8_t)tmv.tm_sec;
    }
    return 0;
}
static int checked_product(uint64_t a,uint64_t b,uint64_t *out){
    if(b && a>UINT64_MAX/b)return -EOVERFLOW;
    *out=a*b;return 0;
}
static int space(void *ctx,fm03_volume *v){
    fm03_posix *fs=(fm03_posix*)ctx;struct statvfs st;int rc;
    if(!fs||fs->rootfd<0||!v)return -EINVAL;
    *v=(fm03_volume){0};
    if(fstatvfs(fs->rootfd,&st)<0)return -errno;
    rc=checked_product((uint64_t)st.f_blocks,(uint64_t)st.f_frsize,&v->total_bytes);
    if(!rc)rc=checked_product((uint64_t)st.f_bfree,(uint64_t)st.f_frsize,&v->free_bytes);
    if(!rc)rc=checked_product((uint64_t)st.f_bavail,(uint64_t)st.f_frsize,&v->available_bytes);
    if(rc)return rc;
    if(v->free_bytes>v->total_bytes)v->free_bytes=v->total_bytes;
    v->valid=1;return 0;
}
static int begin(void *ctx,const char *parent,const char *name){
    fm03_posix *fs=(fm03_posix*)ctx;int pfd,fd,rc;struct stat initial,opened;
    if(!fs||fs->rootfd<0)return -EINVAL;
    end_scan(fs);
    rc=lookup(fs,parent,name,&initial);if(rc)return rc;
    if(!S_ISDIR(initial.st_mode))return -ENOTDIR;
    pfd=open_parent(fs,parent);if(pfd<0)return pfd;
    fd=openat(pfd,name,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);rc=fd<0?-errno:0;
    close(pfd);if(rc)return rc;
    if(fstat(fd,&opened)<0)rc=-errno;
    else if(!S_ISDIR(opened.st_mode)||opened.st_dev!=fs->device||opened.st_ino!=initial.st_ino)rc=-ESTALE;
    if(rc){close(fd);return rc;}
    fs->frames[0]=fdopendir(fd);
    if(!fs->frames[0]){rc=-errno;close(fd);return rc;}
    fs->depth=1;fs->stats=(fm03_scan){0};return 0;
}
static int scan_step(void *ctx,uint32_t budget,fm03_scan *out){
    fm03_posix *fs=(fm03_posix*)ctx;uint32_t n=0;
    if(!fs||!fs->depth||!out||!budget)return -EINVAL;
    if(budget>FM03_SCAN_BATCH_MAX)budget=FM03_SCAN_BATCH_MAX;
    while(n<budget && fs->depth){
        DIR *d=fs->frames[fs->depth-1u];struct dirent *de;struct stat st;
        int fd=dirfd(d);
        errno=0;de=readdir(d);
        if(!de){
            int err=errno;
            if(err)return -err;
            closedir(d);fs->frames[--fs->depth]=NULL;
            continue;
        }
        if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;
        n++;
        if(fs->stats.visited>=FM03_MAX_SCAN_ITEMS)return -E2BIG;
        fs->stats.visited++;
        if(fstatat(fd,de->d_name,&st,AT_SYMLINK_NOFOLLOW)<0)return -errno;
        if(st.st_dev!=fs->device)return -EXDEV;
        if(S_ISREG(st.st_mode)){
            if(st.st_size<0||(uint64_t)st.st_size>UINT64_MAX-fs->stats.bytes)return -EOVERFLOW;
            fs->stats.files++;fs->stats.bytes+=(uint64_t)st.st_size;
        }else if(S_ISDIR(st.st_mode)){
            int child;struct stat opened;
            if(fs->depth>=FM03_MAX_SCAN_DEPTH)return -ELOOP;
            child=openat(fd,de->d_name,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
            if(child<0)return -errno;
            if(fstat(child,&opened)<0){int err=-errno;close(child);return err;}
            if(!S_ISDIR(opened.st_mode)||opened.st_dev!=fs->device||opened.st_ino!=st.st_ino){close(child);return -ESTALE;}
            fs->frames[fs->depth]=fdopendir(child);
            if(!fs->frames[fs->depth]){int err=-errno;close(child);return err;}
            fs->depth++;fs->stats.directories++;
        }else fs->stats.skipped++; /* symlinks, devices: never follow */
    }
    *out=fs->stats;
    return fs->depth?1:0;
}
void fm03_posix_backend(fm03_backend *out){
    if(!out)return;
    out->info=get_info;out->space=space;out->begin=begin;out->step=scan_step;out->end=end_scan;
}
