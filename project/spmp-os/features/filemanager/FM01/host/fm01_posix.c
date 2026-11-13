/* POSIX host test adapter. Device FAT implementation provides the same callback.
 * Read-only: no create/unlink/rename permissions are exposed to FM-01.
 * Directory traversal is anchored to rootfd and rejects links using openat.
 */
#define _POSIX_C_SOURCE 200809L
#include "fm01_posix.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#ifndef O_NOFOLLOW
#error "FM-01 test adapter requires O_NOFOLLOW"
#endif

int fm01_posix_init(fm01_posix *fs,const char *root) {
    if(!fs || !root) return -EINVAL;
    fs->rootfd=open(root,O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_NOFOLLOW);
    return fs->rootfd<0?-errno:0;
}
void fm01_posix_close(fm01_posix *fs) {if(fs && fs->rootfd>=0){close(fs->rootfd);fs->rootfd=-1;}}
static int open_dir_beneath(int rootfd,const char *rel) {
    int fd=dup(rootfd);
    const char *s=rel;
    if(fd<0)return -errno;
    if(!s || *s=='/') {close(fd);return -EINVAL;}
    while(*s) {
        size_t n=0;char segment[FM01_NAME_MAX];int next;
        while(s[n] && s[n]!='/')n++;
        if(!n || n>=sizeof(segment) || (n==1 && s[0]=='.') ||
            (n==2 && s[0]=='.' && s[1]=='.')) {close(fd);return -EINVAL;}
        memcpy(segment,s,n);segment[n]=0;
        next=openat(fd,segment,O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_NOFOLLOW);
        if(next<0) {int code=-errno;close(fd);return code;}
        close(fd);fd=next;
        s+=n;if(*s=='/')s++;
    }
    return fd;
}
static int name_cmp(const void *pa,const void *pb) {
    const fm01_entry *a=(const fm01_entry*)pa,*b=(const fm01_entry*)pb;
    if(a->type!=b->type)return a->type==FM01_ENTRY_DIR?-1:1;
    return strcmp(a->name,b->name); /* deterministic UTF-8-byte ordering */
}
int fm01_posix_list(void *context,const char *relative_path,uint32_t offset,
    uint32_t capacity,fm01_entry *out,uint32_t *returned,uint32_t *total,
    uint32_t *file_count,uint32_t *dir_count) {
    fm01_posix *fs=(fm01_posix*)context;
    DIR *dir;struct dirent *de;fm01_entry *all=0;size_t count=0,alloc=0;
    uint32_t files=0,dirs=0;int dirfd=-1,rc=0;
    if(!fs || fs->rootfd<0 || !returned || !total || !file_count || !dir_count ||
        (capacity && !out) || !relative_path)return -EINVAL;
    *returned=*total=*file_count=*dir_count=0;
    dirfd=open_dir_beneath(fs->rootfd,relative_path);
    if(dirfd<0)return dirfd;
    dir=fdopendir(dirfd);
    if(!dir){rc=-errno;close(dirfd);return rc;}
    errno=0;
    while((de=readdir(dir))) {
        fm01_entry e;struct stat st;struct tm tmv;size_t len;
        if(!strcmp(de->d_name,".") || !strcmp(de->d_name,".."))continue;
        len=strlen(de->d_name);
        if(len==0 || len>=FM01_NAME_MAX){rc=-ENAMETOOLONG;break;}
        if(fstatat(dirfd,de->d_name,&st,AT_SYMLINK_NOFOLLOW)<0){rc=-errno;break;}
        if(!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))continue; /* never follow links */
        memset(&e,0,sizeof(e));
        memcpy(e.name,de->d_name,len+1);
        e.type=S_ISDIR(st.st_mode)?FM01_ENTRY_DIR:FM01_ENTRY_FILE;
        e.size_bytes=S_ISREG(st.st_mode)?(uint64_t)st.st_size:0;
        e.read_only=!(st.st_mode&S_IWUSR);
        if(localtime_r(&st.st_mtime,&tmv)) {
            e.day=(uint8_t)tmv.tm_mday;e.month=(uint8_t)(tmv.tm_mon+1);
            e.year=(uint16_t)(tmv.tm_year+1900);
        }
        if(count==alloc) {
            size_t newcap=alloc?alloc*2:32;
            fm01_entry *next;
            if(newcap>SIZE_MAX/sizeof(*next)){rc=-EOVERFLOW;break;}
            next=(fm01_entry*)realloc(all,newcap*sizeof(*next));
            if(!next){rc=-ENOMEM;break;}
            all=next;alloc=newcap;
        }
        all[count++]=e;
        if(e.type==FM01_ENTRY_DIR)dirs++;else files++;
        errno=0;
    }
    if(!rc && errno)rc=-errno;
    closedir(dir);
    if(!rc) {
        uint32_t n=0;size_t i;
        if(count>UINT32_MAX)rc=-EOVERFLOW;
        else {
            if(count>1)qsort(all,count,sizeof(*all),name_cmp);
            *total=(uint32_t)count;*file_count=files;*dir_count=dirs;
            for(i=offset;i<count && n<capacity;i++)out[n++]=all[i];
            *returned=n;
        }
    }
    free(all);
    return rc;
}
