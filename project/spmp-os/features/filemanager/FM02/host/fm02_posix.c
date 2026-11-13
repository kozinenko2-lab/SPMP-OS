/* SPDX-License-Identifier: MIT
 * Linux-only secure test adapter. Use FAT callbacks for actual hardware.
 * Root fd + O_NOFOLLOW; all writes are under opened directory descriptors.
 * No implicit overwrite. Copy stages to hidden sibling and publishes atomically.
 */
#define _GNU_SOURCE
#include "fm02_posix.h"
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef O_NOFOLLOW
#error O_NOFOLLOW is required
#endif
#ifndef RENAME_NOREPLACE
#define RENAME_NOREPLACE 1
#endif
#define DEPTH_MAX 16u

typedef struct {
    fm02_posix *fs;
    fm02_progress state;
    fm02_progress_hook hook;
    void *hook_ctx;
} task;
static int notify(task *t,unsigned phase,int cancellable){
    t->state.phase=(uint8_t)phase;
    if(t->hook && t->hook(t->hook_ctx,&t->state) && cancellable)return -ECANCELED;
    return 0;
}
static int name_ok(const char *n){return fm02_valid_name(n);}
static int safe_fd(fm02_posix *fs,int fd){struct stat st;
    if(fstat(fd,&st)<0)return -errno;
    if(!S_ISDIR(st.st_mode)||st.st_dev!=fs->device)return -EXDEV;
    return 0;
}
int fm02_posix_init(fm02_posix *fs,const char *root){struct stat st;
    if(!fs||!root)return -EINVAL;
    fs->rootfd=open(root,O_DIRECTORY|O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    if(fs->rootfd<0)return -errno;
    if(fstat(fs->rootfd,&st)<0){int e=-errno;close(fs->rootfd);fs->rootfd=-1;return e;}
    fs->device=st.st_dev;fs->seq=0;return 0;
}
void fm02_posix_close(fm02_posix *fs){if(fs&&fs->rootfd>=0){close(fs->rootfd);fs->rootfd=-1;}}
static int open_subdir(fm02_posix *fs,const char *rel){
    size_t i=0;int fd,rc;
    if(!rel||rel[0]=='/'||strlen(rel)>=FM01_PATH_MAX)return -EINVAL;
    fd=dup(fs->rootfd);if(fd<0)return -errno;
    while(rel[i]){
        char name[FM01_NAME_MAX];size_t j=0;int next;
        while(rel[i]&&rel[i]!='/'){
            if(j+1>=sizeof(name)){close(fd);return -ENAMETOOLONG;}
            name[j++]=rel[i++];
        }
        name[j]=0;
        if(!name_ok(name) || (rel[i]=='/'&&!rel[i+1])){close(fd);return -EINVAL;}
        if(rel[i]=='/')i++;
        next=openat(fd,name,O_DIRECTORY|O_RDONLY|O_NOFOLLOW|O_CLOEXEC);
        if(next<0){rc=-errno;close(fd);return rc;}
        close(fd);fd=next;
        rc=safe_fd(fs,fd);if(rc){close(fd);return rc;}
    }
    return fd;
}
static int check_entry(fm02_posix *fs,int parent,const char *name,struct stat *st){
    if(!name_ok(name))return -EINVAL;
    if(fstatat(parent,name,st,AT_SYMLINK_NOFOLLOW)<0)return -errno;
    if(st->st_dev!=fs->device)return -EXDEV;
    if(!S_ISREG(st->st_mode)&&!S_ISDIR(st->st_mode))return -EOPNOTSUPP;
    return 0;
}
static int no_replace_rename(int a,const char *from,int b,const char *to){
#if defined(SYS_renameat2)
    if(syscall(SYS_renameat2,a,from,b,to,RENAME_NOREPLACE)==0)return 0;
    return -errno;
#else
    (void)a;(void)from;(void)b;(void)to;return -ENOSYS;
#endif
}
static int sync_directory(int fd){if(fsync(fd)<0)return -errno;return 0;}
static int count_tree(task *t,int parent,const char *name,unsigned depth){
    struct stat st;int rc=check_entry(t->fs,parent,name,&st);
    if(rc)return rc;
    if(depth>DEPTH_MAX)return -ELOOP;
    if(t->state.items_total==UINT32_MAX)return -EOVERFLOW;
    t->state.items_total++;
    if(S_ISREG(st.st_mode)){
        if(st.st_size<0 || (uint64_t)st.st_size>UINT64_MAX-t->state.bytes_total)return -EOVERFLOW;
        t->state.bytes_total+=(uint64_t)st.st_size;
    }else{
        int dfd=openat(parent,name,O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC),err=0;
        DIR *dir;struct dirent *de;
        if(dfd<0)return -errno;
        rc=safe_fd(t->fs,dfd);if(rc){close(dfd);return rc;}
        dir=fdopendir(dfd);if(!dir){rc=-errno;close(dfd);return rc;}
        for(;;){errno=0;de=readdir(dir);
            if(!de){if(errno)err=-errno;break;}
            if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;
            rc=count_tree(t,dfd,de->d_name,depth+1);
            if(rc){err=rc;break;}
        }
        closedir(dir);
        if(err)return err;
    }
    if((t->state.items_total&31u)==0)return notify(t,1,1);
    return 0;
}
static int unique_stage(fm02_posix *fs,int parent,int directory,char *name,size_t cap,int *opened){
    unsigned j;*opened=-1;
    for(j=0;j<1000;j++){
        int n=snprintf(name,cap,".spmpfm02-%08x-%04u",fs->seq++,j);
        if(n<=0||(size_t)n>=cap)return -ENAMETOOLONG;
        if(directory){
            if(mkdirat(parent,name,0700)==0){*opened=openat(parent,name,O_DIRECTORY|O_NOFOLLOW|O_RDONLY|O_CLOEXEC);
                if(*opened<0){int err=-errno;unlinkat(parent,name,AT_REMOVEDIR);return err;}
                return 0;}
        }else {
            *opened=openat(parent,name,O_CREAT|O_EXCL|O_WRONLY|O_CLOEXEC|O_NOFOLLOW,0600);
            if(*opened>=0)return 0;
        }
        if(errno!=EEXIST)return -errno;
    }
    return -EEXIST;
}
/* Remove an internal staging tree; never traverses symlinks. */
static int remove_tree(int parent,const char *name,unsigned depth){
    struct stat st;int fd,rc=0;
    if(depth>DEPTH_MAX+1u)return -ELOOP;
    if(fstatat(parent,name,&st,AT_SYMLINK_NOFOLLOW)<0)return -errno;
    if(S_ISDIR(st.st_mode)){
        DIR *dir;struct dirent *de;int err=0;
        fd=openat(parent,name,O_DIRECTORY|O_RDONLY|O_NOFOLLOW|O_CLOEXEC);
        if(fd<0)return -errno;
        dir=fdopendir(fd);if(!dir){rc=-errno;close(fd);return rc;}
        for(;;){errno=0;de=readdir(dir);if(!de){if(errno)err=-errno;break;}
            if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;
            rc=remove_tree(fd,de->d_name,depth+1);
            if(rc){err=rc;break;}
        }
        closedir(dir);if(err)return err;
        if(unlinkat(parent,name,AT_REMOVEDIR)<0)return -errno;
    }else if(unlinkat(parent,name,0)<0)return -errno;
    return 0;
}
static int copy_file(task *t,int src_parent,const char *src_name,int dst_parent,
                     const char *dst_name,int existing_dst_fd){
    struct stat expect,actual;unsigned char buf[FM02_IO_CHUNK];int src,rc;
    if((rc=check_entry(t->fs,src_parent,src_name,&expect))!=0)return rc;
    if(!S_ISREG(expect.st_mode))return -EISDIR;
    src=openat(src_parent,src_name,O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
    if(src<0)return -errno;
    if(fstat(src,&actual)<0){rc=-errno;goto end;}
    if(!S_ISREG(actual.st_mode)||actual.st_dev!=expect.st_dev||actual.st_ino!=expect.st_ino){rc=-ESTALE;goto end;}
    for(;;){ssize_t n=read(src,buf,sizeof(buf));size_t off=0;
        if(n<0){if(errno==EINTR)continue;rc=-errno;goto end;}
        if(n==0)break;
        while(off<(size_t)n){ssize_t w=write(existing_dst_fd,buf+off,(size_t)n-off);
            if(w<0){if(errno==EINTR)continue;rc=-errno;goto end;}
            if(w==0){rc=-EIO;goto end;}
            off+=(size_t)w;
        }
        t->state.bytes_done+=(uint64_t)n;
        if((rc=notify(t,2,1))!=0)goto end;
    }
    {
        struct stat after;
        if(fstat(src,&after)<0){rc=-errno;goto end;}
        if(after.st_dev!=actual.st_dev || after.st_ino!=actual.st_ino ||
           after.st_size!=actual.st_size ||
           after.st_mtim.tv_sec!=actual.st_mtim.tv_sec ||
           after.st_mtim.tv_nsec!=actual.st_mtim.tv_nsec){rc=-ESTALE;goto end;}
    }
    if(fchmod(existing_dst_fd,actual.st_mode&0777u)<0){rc=-errno;goto end;}
    {
        const struct timespec original_times[2]={expect.st_atim,expect.st_mtim};
        if(futimens(existing_dst_fd,original_times)<0){rc=-errno;goto end;}
    }
    if(fsync(existing_dst_fd)<0){rc=-errno;goto end;}
    t->state.items_done++;
    rc=notify(t,2,1);
end:
    (void)dst_parent;(void)dst_name;
    close(src);return rc;
}
static int copy_directory(task *t,int src_parent,const char *src_name,int dst_dir,unsigned depth){
    int srcfd,rc,err=0;DIR *dir;struct dirent *de;
    if(depth>DEPTH_MAX)return -ELOOP;
    srcfd=openat(src_parent,src_name,O_DIRECTORY|O_RDONLY|O_NOFOLLOW|O_CLOEXEC);
    if(srcfd<0)return -errno;
    rc=safe_fd(t->fs,srcfd);if(rc){close(srcfd);return rc;}
    dir=fdopendir(srcfd);if(!dir){rc=-errno;close(srcfd);return rc;}
    for(;;){struct stat st;int child=-1;
        errno=0;de=readdir(dir);if(!de){if(errno)err=-errno;break;}
        if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;
        rc=check_entry(t->fs,srcfd,de->d_name,&st);if(rc){err=rc;break;}
        if(S_ISDIR(st.st_mode)){
            if(mkdirat(dst_dir,de->d_name,0700)<0){err=-errno;break;}
            child=openat(dst_dir,de->d_name,O_DIRECTORY|O_RDONLY|O_NOFOLLOW|O_CLOEXEC);
            if(child<0){err=-errno;break;}
            rc=copy_directory(t,srcfd,de->d_name,child,depth+1);
            if(!rc && fchmod(child,st.st_mode&0777u)<0)rc=-errno;
            if(!rc){
                const struct timespec original_times[2]={st.st_atim,st.st_mtim};
                if(futimens(child,original_times)<0)rc=-errno;
            }
            if(!rc && fsync(child)<0)rc=-errno;
            close(child);
            if(rc){err=rc;break;}
        }else{
            child=openat(dst_dir,de->d_name,O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC|O_WRONLY,0600);
            if(child<0){err=-errno;break;}
            rc=copy_file(t,srcfd,de->d_name,dst_dir,de->d_name,child);
            close(child);
            if(rc){err=rc;break;}
        }
    }
    closedir(dir);
    if(err)return err;
    t->state.items_done++;
    return notify(t,2,1);
}
static int path_is_descendant(const char *base,const char *name,const char *dst){
    char full[FM01_PATH_MAX+FM01_NAME_MAX+2];size_t n=strlen(base),m=strlen(name),d=strlen(dst);
    if(n+m+2>sizeof(full))return 1;
    memcpy(full,base,n);if(n)full[n++]='/';memcpy(full+n,name,m);n+=m;full[n]=0;
    return (d>=n && !memcmp(full,dst,n) && (d==n||dst[n]=='/'));
}
int fm02_posix_action(void *context,const fm02_request *req,
    fm02_progress_hook hook,void *hook_ctx,fm02_progress *result){
    fm02_posix *fs=(fm02_posix*)context;task t;int src=-1,dst=-1,rc=0;
    struct stat st;char tmp[64]={0};int tempfd=-1,staged=0;
    if(!fs||fs->rootfd<0||!req||!result||!name_ok(req->name))return -EINVAL;
    if(req->action<FM02_COPY||req->action>FM02_DELETE)return -EINVAL;
    if(req->action==FM02_RENAME&&!name_ok(req->new_name))return -EINVAL;
    memset(&t,0,sizeof(t));t.fs=fs;t.hook=hook;t.hook_ctx=hook_ctx;
    src=open_subdir(fs,req->src_directory);if(src<0)return src;
    if(req->action==FM02_COPY||req->action==FM02_MOVE){
        dst=open_subdir(fs,req->dst_directory);if(dst<0){rc=dst;goto end;}
        if(path_is_descendant(req->src_directory,req->name,req->dst_directory)){
            rc=-EINVAL;goto end;
        }
    }
    rc=check_entry(fs,src,req->name,&st);if(rc)goto end;
    if(req->action==FM02_COPY || req->action==FM02_DELETE){
        rc=count_tree(&t,src,req->name,0);if(rc)goto end;
        rc=notify(&t,1,1);if(rc)goto end;
    }
    if(req->action==FM02_MOVE || req->action==FM02_RENAME){
        rc=notify(&t,3,1);if(rc)goto end;
        rc=no_replace_rename(src,req->name,req->action==FM02_MOVE?dst:src,
                             req->action==FM02_MOVE?req->name:req->new_name);
        if(!rc){t.state.items_done=1;t.state.items_total=1;t.state.phase=3;
            (void)sync_directory(src);if(dst>=0)(void)sync_directory(dst);
            (void)notify(&t,3,0);
        }
        goto end;
    }
    if(req->action==FM02_COPY){
        rc=unique_stage(fs,dst,S_ISDIR(st.st_mode),tmp,sizeof(tmp),&tempfd);
        if(rc)goto end;
        staged=1;
        if(S_ISDIR(st.st_mode)){
            rc=copy_directory(&t,src,req->name,tempfd,0);
            if(!rc && fchmod(tempfd,st.st_mode&0777u)<0)rc=-errno;
            if(!rc){
                const struct timespec original_times[2]={st.st_atim,st.st_mtim};
                if(futimens(tempfd,original_times)<0)rc=-errno;
            }
            if(!rc && fsync(tempfd)<0)rc=-errno;
        }else rc=copy_file(&t,src,req->name,dst,tmp,tempfd);
        close(tempfd);tempfd=-1;
        if(rc)goto end;
        if(t.state.items_done!=t.state.items_total || t.state.bytes_done!=t.state.bytes_total){
            rc=-ESTALE;goto end;
        }
        rc=notify(&t,3,1);if(rc)goto end;
        rc=no_replace_rename(dst,tmp,dst,req->name);
        if(rc)goto end;
        staged=0;
        (void)sync_directory(dst);(void)notify(&t,3,0);
        goto end;
    }
    /* Delete has been explicitly confirmed by UI. Cancellation is accepted
       during preflight; after commit starts removal is irreversible. */
    rc=unique_stage(fs,src,S_ISDIR(st.st_mode),tmp,sizeof(tmp),&tempfd);
    if(rc)goto end;
    close(tempfd);tempfd=-1;staged=1;
    if(unlinkat(src,tmp,S_ISDIR(st.st_mode)?AT_REMOVEDIR:0)<0){rc=-errno;goto end;}
    staged=0;
    rc=no_replace_rename(src,req->name,src,tmp);
    if(rc)goto end;
    staged=1;
    (void)sync_directory(src);
    t.state.phase=4;(void)notify(&t,4,0);
    rc=remove_tree(src,tmp,0);
    if(!rc){staged=0;t.state.bytes_done=t.state.bytes_total;
        t.state.items_done=t.state.items_total;
        (void)sync_directory(src);(void)notify(&t,4,0);
    }else{
        /* Partial deletion cannot be undone. Try to retain remaining files
           under original name, but caller must report the error. */
        if(!no_replace_rename(src,tmp,src,req->name))staged=0;
    }
end:
    if(tempfd>=0)close(tempfd);
    if(staged && req->action==FM02_COPY){(void)remove_tree(dst,tmp,0);}
    *result=t.state;
    if(src>=0)close(src);
    if(dst>=0)close(dst);
    return rc;
}
