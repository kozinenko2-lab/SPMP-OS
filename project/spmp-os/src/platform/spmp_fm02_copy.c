/* SPDX-License-Identifier: MIT */
#include "spmp_fm02_copy.h"
static int join(char out[SPMP_COPY_PATH],const char *dir,const char *name){
 unsigned n=0,k=0;if(!dir||!name)return -22;
 while(n<SPMP_COPY_PATH&&dir[n]){out[n]=dir[n];n++;}
 if(n+1>=SPMP_COPY_PATH)return -36;
 if(n)out[n++]='/';
 while(n+1<SPMP_COPY_PATH&&name[k])out[n++]=name[k++];
 out[n]=0;return name[k]?-36:0;
}
static void report(const struct spmp_file_copy *t,fm02_progress *p){
 p->bytes_done=(uint64_t)t->written+t->verified;p->bytes_total=(uint64_t)t->original.bytes*2u;
 p->items_total=1;p->items_done=t->published?1:0;
 if(t->phase<SPMP_COPY_DONE)p->phase=t->phase<SPMP_COPY_READ?1:t->phase==SPMP_COPY_PUBLISH?3:2;
 else if(t->published)p->phase=3; /* Failure retains its actual last IO phase. */
}
int spmp_fm02_copy_action(void *ctx,const fm02_request *req,fm02_progress_hook hook,void *hookctx,fm02_progress *out){
 struct spmp_fm02_copy *c=ctx;char src[SPMP_COPY_PATH],dst[SPMP_COPY_PATH];
 if(!c||!req||!out)return -22;
 *out=(fm02_progress){0};if(req->action!=FM02_COPY)return -38;
 if(!fm02_valid_name(req->name))return -22;
 int r=join(src,req->src_directory,req->name);if(r)return r;
 r=join(dst,req->dst_directory,req->name);if(r)return r;
 r=spmp_file_copy_start(&c->task,src,dst,c->sequence++);if(r)return r;
 for(;;){
  report(&c->task,out);if(hook&&hook(hookctx,out))spmp_file_copy_cancel(&c->task);
  if(spmp_file_copy_step(&c->task))break;
 }
 report(&c->task,out);if(hook)(void)hook(hookctx,out);
 return c->task.error;
}
