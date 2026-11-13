#define _DEFAULT_SOURCE
#include "defos.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

// Strictly read-only. Traversal is built only from directory entries and
// symlinks are never visited, including symlinked directories.
typedef struct {char root[PATH_MAX];} host_fs;
static host_fs state;
void *host_fs_init(const char *root) {
 if(!realpath(root,state.root))return NULL;
 struct stat st;
 if(stat(state.root,&st)||!S_ISDIR(st.st_mode))return NULL;
 return &state;
}
int host_fs_list(void *ctx,const char *rel,defos_entry *out,size_t max) {
 host_fs *fs=(host_fs*)ctx;char path[PATH_MAX];
 if(!fs||!rel||rel[0]=='/'||strstr(rel,".."))return -1;
 if(snprintf(path,sizeof path,"%s/%s",fs->root,rel)>=(int)sizeof path)return -1;
 DIR *dir=opendir(path);if(!dir)return -1;
 struct dirent *de;size_t n=0;
 while((de=readdir(dir))&&n<max){
  if(!strcmp(de->d_name,".")||!strcmp(de->d_name,".."))continue;
  if(strlen(de->d_name)>DEFOS_NAME_MAX)continue;
  char path2[PATH_MAX];
  if(snprintf(path2,sizeof path2,"%s/%s",path,de->d_name)>=(int)sizeof path2)continue;
  struct stat st;
  if(lstat(path2,&st)!=0 || S_ISLNK(st.st_mode))continue;
  if(!S_ISDIR(st.st_mode)&&!S_ISREG(st.st_mode))continue;
  defos_entry *e=&out[n++];
  snprintf(e->name,sizeof e->name,"%s",de->d_name);
  e->kind=defos_classify(e->name,S_ISDIR(st.st_mode));
  e->size=S_ISDIR(st.st_mode)?0:(uint64_t)st.st_size;
 }
 closedir(dir);return (int)n;
}
