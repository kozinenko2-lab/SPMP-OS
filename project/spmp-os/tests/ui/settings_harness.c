#include "spmp_settings.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {uint8_t buf[SPMP_SETTINGS_DISK_BYTES];char path[64];size_t length;int stores;int fail;int called;} storage;
static int load(void *ctx,const char*path,uint8_t*dst,size_t cap,size_t*n){
 storage*s=ctx;if(!s->length||strcmp(path,s->path)||cap<s->length)return -1;
 memcpy(dst,s->buf,s->length);*n=s->length;return 0;
}
static int save(void *ctx,const char*path,const uint8_t *src,size_t n){
 storage*s=ctx;if(s->fail||n>sizeof s->buf)return -1;
 memcpy(s->buf,src,n);s->length=n;snprintf(s->path,sizeof s->path,"%s",path);s->stores++;return 0;
}
static int apply(void*ctx,spmp_setting_id id,int val){
 storage*s=ctx;(void)val;s->called++;
 return id==SPMP_OPT_VOLUME||id==SPMP_OPT_SWAP_AB?0:1;
}
int main(void){
 storage st={0};spmp_settings_io io={&st,load,save,apply};
 spmp_settings a,b,c;assert(!spmp_settings_init(&a,"gb",&io));
 assert(a.count==6&&spmp_settings_get(&a,SPMP_OPT_VOLUME,-1)==8);
 assert(!spmp_settings_change(&a,1,1));assert(a.values[1]==9&&st.stores==1);
 assert(!spmp_settings_change(&a,1,1));assert(a.values[1]==10);
 assert(!spmp_settings_change(&a,1,1));assert(a.values[1]==0);
 assert(!spmp_settings_change(&a,2,1));assert(a.values[2]==1);
 assert(!spmp_settings_change(&a,4,1));assert(a.values[4]==1);
 assert(!spmp_settings_init(&b,"gb",&io));assert(!spmp_settings_read(&b));
 assert(b.values[1]==0&&b.values[2]==1&&b.values[4]==1);
 assert(!spmp_settings_init(&c,"nes",&io));assert(spmp_settings_read(&c)==1);
 assert(strcmp(c.path,b.path));assert(c.values[1]==8);
 int before=b.values[1];st.fail=1;
 assert(spmp_settings_change(&b,1,1)==-1&&b.values[1]==before);st.fail=0;
 st.buf[20]^=1;assert(spmp_settings_read(&b)==-1&&b.values[1]==before);
 st.buf[20]^=1;
 assert(!spmp_settings_read(&b));
 char val[20];assert(!strcmp(spmp_settings_value(&b,1,val,sizeof val),"0/10"));
 printf("SPMP settings: per-core identity, wrap, rollback, checksum, save/load, deferred settings OK\n");
 return 0;
}
