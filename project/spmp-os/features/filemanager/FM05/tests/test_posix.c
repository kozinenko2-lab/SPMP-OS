/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include "fm05_posix.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
static void write_full(const char *path,const char *s,size_t n){
    int fd=open(path,O_CREAT|O_TRUNC|O_WRONLY,0644);assert(fd>=0);
    size_t o=0;while(o<n){ssize_t z=write(fd,s+o,n-o);assert(z>0);o+=(size_t)z;}
    assert(close(fd)==0);
}
static size_t read_full(const char *path,char *s,size_t cap){
    int fd=open(path,O_RDONLY);size_t o=0;assert(fd>=0);
    for(;;){ssize_t z=read(fd,s+o,cap-o);assert(z>=0);if(z==0)break;o+=(size_t)z;assert(o<cap);}
    assert(close(fd)==0);return o;
}
int main(void){
    char root[]="/tmp/spmp-fm05-XXXXXX",sub[500],file[500],outside[500],link[500],large[500];
    fm05_posix b;fm05_doc d;fm05_fs fs;char buf[40000];size_t n;
    assert(mkdtemp(root));
    assert(snprintf(sub,sizeof(sub),"%s/docs",root)>0);assert(mkdir(sub,0755)==0);
    assert(snprintf(file,sizeof(file),"%s/docs/тест.log",root)>0);write_full(file,"Первый\nВторой\n",strlen("Первый\nВторой\n"));
    assert(snprintf(outside,sizeof(outside),"%s/external.txt",root)>0);write_full(outside,"not changed",11);
    assert(snprintf(link,sizeof(link),"%s/docs/link.txt",root)>0);assert(symlink(outside,link)==0);
    assert(snprintf(large,sizeof(large),"%s/docs/huge.txt",root)>0);memset(buf,'A',FM05_TEXT_CAP+2u);write_full(large,buf,FM05_TEXT_CAP+2u);
    assert(fm05_posix_init(&b,root)==0);
    fs=(fm05_fs){fm05_posix_load,fm05_posix_save,&b};fm05_init(&d,fs);
    assert(fm05_open(&d,"docs/тест.log",0)==0);
    fm05_line_end(&d);assert(fm05_insert(&d," /доп",strlen(" /доп"))==0);
    assert(fm05_save(&d)==0);
    n=read_full(file,buf,sizeof(buf));buf[n]=0;assert(strstr(buf," /доп")!=0);
    assert(fm05_open(&d,"docs/тест.log",0)==0);
    assert(fm05_insert(&d,"X",1)==0);
    write_full(file,"external modification",21);
    assert(fm05_save(&d)==FM05_ERR_CONFLICT);
    n=read_full(file,buf,sizeof(buf));buf[n]=0;assert(strcmp(buf,"external modification")==0);
    assert(fm05_open(&d,"docs/link.txt",0)==FM05_ERR_CONFLICT); /* dirty not discarded */
    assert(fm05_request_close(&d)==0);fm05_confirm_close(&d,1);
    assert(fm05_open(&d,"docs/link.txt",0)<0);
    assert(fm05_open(&d,"../external.txt",0)<0);
    assert(fm05_open(&d,"docs/huge.txt",0)==FM05_ERR_TOO_LARGE);
    assert(fm05_open(&d,"docs/тест.log",0)==0);
    assert(fm05_insert(&d,"Y",1)==0);
    assert(fm05_save(&d)==0);
    /* Detect pathname symlink substituted between opening and saving. */
    assert(fm05_open(&d,"docs/тест.log",0)==0);
    assert(fm05_insert(&d,"Z",1)==0);
    assert(unlink(file)==0);assert(symlink(outside,file)==0);
    assert(fm05_save(&d)<0);
    n=read_full(outside,buf,sizeof(buf));buf[n]=0;assert(strcmp(buf,"not changed")==0);
    fm05_posix_close(&b);
    assert(unlink(file)==0);assert(unlink(link)==0);assert(unlink(large)==0);
    assert(unlink(outside)==0);assert(rmdir(sub)==0);assert(rmdir(root)==0);
    puts("FM05 POSIX: atomic temp/rename, conflict, traversal, symlink, size guard OK");return 0;
}
