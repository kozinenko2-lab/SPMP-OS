/* SPDX-License-Identifier: MIT
 * POSIX testing backend. The Defender FAT backend must implement fm05_fs
 * callbacks separately. The host backend never follows file/directory links.
 */
#define _POSIX_C_SOURCE 200809L
#include "fm05_posix.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef O_NOFOLLOW
#error "FM05 POSIX requires O_NOFOLLOW"
#endif
static int open_parent(int rootfd,const char *path,char *name,unsigned name_cap){
    int dir=-1,nextfd;const char *s,*slash;unsigned n;
    if(rootfd<0||!path||!path[0]||path[0]=='/')return -EINVAL;
    dir=openat(rootfd,".",O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
    if(dir<0)return -errno;
    s=path;
    while((slash=strchr(s,'/'))!=NULL){
        n=(unsigned)(slash-s);
        if(n==0u||n>=FM01_NAME_MAX||(n==1u&&s[0]=='.')||(n==2u&&s[0]=='.'&&s[1]=='.')){close(dir);return -EINVAL;}
        char segment[FM01_NAME_MAX];memcpy(segment,s,n);segment[n]=0;
        nextfd=openat(dir,segment,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
        if(nextfd<0){int e=-errno;close(dir);return e;}
        close(dir);dir=nextfd;s=slash+1;
    }
    n=(unsigned)strlen(s);
    if(!n||n>=name_cap||(n==1u&&s[0]=='.')||(n==2u&&s[0]=='.'&&s[1]=='.')){close(dir);return -EINVAL;}
    memcpy(name,s,n+1u);return dir;
}
static uint64_t revision(const struct stat *st){
    /* Best-effort change detection. This is a non-cryptographic revision tag. */
    uint64_t a=(uint64_t)st->st_ino,b=(uint64_t)st->st_size;
    uint64_t c=(uint64_t)(uintmax_t)st->st_mtim.tv_sec;
    uint64_t d=(uint64_t)st->st_mtim.tv_nsec;
    uint64_t e=(uint64_t)(uintmax_t)st->st_ctim.tv_sec;
    uint64_t f=(uint64_t)st->st_ctim.tv_nsec;
    return (a*0x9e3779b185ebca87ULL)^(b*0xc2b2ae3d27d4eb4fULL)^
           (c*0x165667b19e3779f9ULL)^(d<<20)^(e*0x27d4eb2f165667c5ULL)^f;
}
int fm05_posix_init(fm05_posix *ctx,const char *root){
    if(!ctx||!root)return -EINVAL;
    ctx->rootfd=open(root,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
    ctx->temp_sequence=0;
    return ctx->rootfd<0?-errno:0;
}
void fm05_posix_close(fm05_posix *ctx){if(ctx&&ctx->rootfd>=0){close(ctx->rootfd);ctx->rootfd=-1;}}
int fm05_posix_load(void *context,const char *path,char *out,unsigned cap,unsigned *n,uint64_t *rev){
    fm05_posix *ctx=(fm05_posix*)context;char name[FM01_NAME_MAX];
    int dir,fd,rc=0;struct stat a,b;unsigned count=0;
    if(!ctx||!out||!n||!rev)return -EINVAL;
    *n=0;*rev=0;dir=open_parent(ctx->rootfd,path,name,sizeof(name));if(dir<0)return dir;
    fd=openat(dir,name,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    if(fd<0){rc=-errno;close(dir);return rc;}
    if(fstat(fd,&a)<0){rc=-errno;goto done;}
    if(!S_ISREG(a.st_mode)){rc=-EINVAL;goto done;}
    if(a.st_size>(off_t)cap){rc=FM05_ERR_TOO_LARGE;goto done;}
    while(count<cap){
        ssize_t z=read(fd,out+count,cap-count);
        if(z<0){if(errno==EINTR)continue;rc=-errno;goto done;}
        if(!z)break;count+=(unsigned)z;
    }
    if(count==cap){char byte;ssize_t z=read(fd,&byte,1);if(z>0){rc=FM05_ERR_TOO_LARGE;goto done;}if(z<0){rc=-errno;goto done;}}
    if(fstat(fd,&b)<0){rc=-errno;goto done;}
    if(revision(&a)!=revision(&b)){rc=FM05_ERR_CONFLICT;goto done;}
    *n=count;*rev=revision(&b);
done:close(fd);close(dir);return rc;
}
int fm05_posix_save(void *context,const char *path,const char *text,unsigned size,
    uint64_t expected,uint64_t *updated){
    fm05_posix *ctx=(fm05_posix*)context;char name[FM01_NAME_MAX],tmpname[64];
    int dir,orig=-1,tmp=-1,rc=0,created=0;struct stat st;
    unsigned tries;size_t off=0;
    if(!ctx||!text||!updated||size>FM05_TEXT_CAP)return -EINVAL;
    dir=open_parent(ctx->rootfd,path,name,sizeof(name));if(dir<0)return dir;
    orig=openat(dir,name,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    if(orig<0){rc=-errno;goto done;}
    if(fstat(orig,&st)<0){rc=-errno;goto done;}
    if(!S_ISREG(st.st_mode)){rc=-EINVAL;goto done;}
    if(revision(&st)!=expected){rc=FM05_ERR_CONFLICT;goto done;}
    for(tries=0;tries<100u;tries++){
        (void)snprintf(tmpname,sizeof(tmpname),".spmp-fm05-%ld-%u.tmp",(long)getpid(),ctx->temp_sequence++);
        tmp=openat(dir,tmpname,O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);
        if(tmp>=0){created=1;break;}
        if(errno!=EEXIST){rc=-errno;goto done;}
    }
    if(tmp<0){rc=-EEXIST;goto done;}
    if(fchmod(tmp,st.st_mode&0777)<0){rc=-errno;goto done;}
    while(off<size){ssize_t z=write(tmp,text+off,size-off);if(z<0){if(errno==EINTR)continue;rc=-errno;goto done;}if(z==0){rc=-EIO;goto done;}off+=(size_t)z;}
    if(fsync(tmp)<0){rc=-errno;goto done;}
    if(close(tmp)<0){tmp=-1;rc=-errno;goto done;}tmp=-1;
    /* Reject any already-observed concurrent modification. POSIX cannot make
     * revision-check + rename atomic against non-cooperating processes. */
    if(fstatat(dir,name,&st,AT_SYMLINK_NOFOLLOW)<0){rc=-errno;goto done;}
    if(!S_ISREG(st.st_mode)||revision(&st)!=expected){rc=FM05_ERR_CONFLICT;goto done;}
    if(renameat(dir,tmpname,dir,name)<0){rc=-errno;goto done;}
    created=0;
    if(fsync(dir)<0){rc=-errno;goto done;}
    if(fstatat(dir,name,&st,AT_SYMLINK_NOFOLLOW)<0){rc=-errno;goto done;}
    *updated=revision(&st);
done:
    if(tmp>=0)close(tmp);
    if(created)unlinkat(dir,tmpname,0);
    if(orig>=0)close(orig);
    close(dir);return rc;
}
