/* Host-only loader of the SMSPlusGX shared module. Native ARMv5 will use
 * the SPMP-OS overlay manager, not dlopen. */
#include "spmp_cores.h"
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static void *handle;
static const spmp_core_ops *underlying;
int spmp_sms_module_open(void){
 if(handle)return 0;
 const char *path=getenv("SPMP_SMS_SO");
 if(!path||!*path)path="third_party/smsplus/libsms_spmp.so";
 handle=dlopen(path,RTLD_NOW|RTLD_LOCAL);
 if(!handle){fprintf(stderr,"SMSPlusGX dlopen: %s\n",dlerror());return -1;}
 underlying=(const spmp_core_ops*)dlsym(handle,"spmp_sms_ops");
 if(!underlying||!underlying->start||!underlying->tick||!underlying->stop){
  dlclose(handle);handle=NULL;underlying=NULL;return -1;
 }
 return 0;
}
void spmp_sms_module_close(void){
 if(handle){dlclose(handle);handle=NULL;underlying=NULL;}
}
static int start(const spmp_core_host*h,const char*p){return underlying?underlying->start(h,p):-1;}
static int tick(void){return underlying?underlying->tick():-1;}
static void stop(void){if(underlying)underlying->stop();}
static size_t size(void){return underlying&&underlying->state_size?underlying->state_size():0;}
static int save(void*p,size_t n){return underlying&&underlying->serialize?underlying->serialize(p,n):-1;}
static int load(const void*p,size_t n){return underlying&&underlying->unserialize?underlying->unserialize(p,n):-1;}
static int set(unsigned k,int v){return underlying&&underlying->set_option?underlying->set_option(k,v):1;}
const spmp_core_ops spmp_sms_ops={start,tick,stop,size,save,load,set};

unsigned spmp_sms_debug_ram0(void){
 if(!handle)return 0;
 void *f=dlsym(handle,"spmp_sms_debug_ram0");
 unsigned (*get)(void)=NULL;
 if(!f)return 0;
 memcpy(&get,&f,sizeof get);
 return get?get():0;
}
