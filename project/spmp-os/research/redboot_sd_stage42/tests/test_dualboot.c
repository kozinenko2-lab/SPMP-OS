#include "spmp_dualboot.h"
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int force, no_card, reject_ram, stage_fail, jump_returns, restore_fail;
    int read_calls, preflight_calls, stage_calls, sd_jumps, factory_jumps, restore_calls;
    enum spmp_db_event last;
    uint8_t pkg[512];
    size_t n;
} Test;
static jmp_buf jump_done;
static void put32(uint8_t *p, uint32_t x) {
    p[0]=(uint8_t)x; p[1]=(uint8_t)(x>>8); p[2]=(uint8_t)(x>>16); p[3]=(uint8_t)(x>>24);
}
static void make_package(Test *t) {
    uint8_t *p=t->pkg; memset(t->pkg,0,sizeof(t->pkg));
    memcpy(p,"SPMPDB01",8);put32(p+8,1);put32(p+12,48);
    put32(p+16,64);put32(p+24,0x1234);put32(p+28,1);
    memcpy(p+48,"\x7f" "ELF\x01\x01\x01",7);
    p[48+18]=40; p[48+19]=0;
    put32(p+20,spmp_db_crc32(p+48,64));t->n=112;
}
static int force(void *c){return ((Test*)c)->force;}
static int read_pkg(void *c,const char *path,uint8_t *to,size_t capacity,size_t *n){
    Test*t=c;t->read_calls++;
    assert(strcmp(path,SPMP_DB_DEFAULT_PATH)==0);
    if(t->no_card || capacity<t->n)return 0;
    memcpy(to,t->pkg,t->n);*n=t->n;return 1;
}
static int preflight(void*c,const uint8_t *img,size_t n,uint32_t *entry){
    Test*t=c;t->preflight_calls++;assert(n==64 && img!=0);
    if(t->reject_ram)return 0;*entry=0x40000;return 1;
}
static int stage(void*c,const uint8_t *img,size_t n,uint32_t *entry){
    Test*t=c;t->stage_calls++;assert(n==64 && img!=0);
    if(t->stage_fail)return 0;*entry=0x40000;return 1;
}
static void jump_sd(void*c,uint32_t entry){
    Test*t=c;t->sd_jumps++;assert(entry==0x40000);
    if(!t->jump_returns)longjmp(jump_done,1);
}
static int restore(void*c){Test*t=c;t->restore_calls++;return t->restore_fail?0:1;}
static void factory(void*c){((Test*)c)->factory_jumps++;}
static void notify(void*c,enum spmp_db_event e){((Test*)c)->last=e;}
static spmp_db_ops ops(Test *t){
    spmp_db_ops o={0};o.context=t;o.verified_board_id=0x1234;
    o.force_factory=force;o.read_package=read_pkg;
    o.restore_factory_environment=restore;o.preflight_elf=preflight;
    o.stage_elf=stage;o.jump_sd=jump_sd;o.boot_factory=factory;o.on_event=notify;
    return o;
}
static void scenario(Test *t,int alt_expected,int read_expected,int stage_expected){
    uint8_t staging[512];spmp_db_ops o=ops(t);
    if(!setjmp(jump_done)) {
        int rc=spmp_db_boot(&o,staging,sizeof(staging));
        assert(rc==-2);
        assert(!alt_expected);
    } else assert(alt_expected);
    assert(t->sd_jumps==(alt_expected || t->jump_returns ? 1 : 0));
    assert(t->factory_jumps==(alt_expected?0:1));
    assert(t->read_calls==read_expected);
    assert(t->stage_calls==stage_expected);
    assert(t->restore_calls==(alt_expected?0:(read_expected?1:0)));
}
int main(void){
    Test t={0};make_package(&t);scenario(&t,1,1,1);
    memset(&t,0,sizeof(t));make_package(&t);t.force=1;scenario(&t,0,0,0);
    memset(&t,0,sizeof(t));make_package(&t);t.no_card=1;scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);t.pkg[0]='X';scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);put32(t.pkg+8,9);scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);put32(t.pkg+24,0x9999);scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);t.pkg[t.n-1]^=1;scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);put32(t.pkg+16,5555);scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);t.pkg[48+18]=3;scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);t.reject_ram=1;scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);t.stage_fail=1;scenario(&t,0,1,1);
    memset(&t,0,sizeof(t));make_package(&t);t.jump_returns=1;scenario(&t,0,1,1);
    memset(&t,0,sizeof(t));make_package(&t);t.pkg[32]=1;scenario(&t,0,1,0);
    memset(&t,0,sizeof(t));make_package(&t);t.no_card=1;t.restore_fail=1;scenario(&t,0,1,0);
    /* Package verifier rejects every truncated length. */
    make_package(&t);
    for(size_t n=0;n<t.n;n++){
        const uint8_t *img=(const uint8_t*)0x1;
        size_t len=1;
        assert(!spmp_db_verify_package(t.pkg,n,0x1234,&img,&len));
        assert(img==0 && len==0);
    }
    assert(spmp_db_crc32((const uint8_t*)"123456789",9)==0xcbf43926u);
    /* Deterministic mutation sweep, checked again with sanitizers. */
    make_package(&t);
    uint32_t seed=0x0b007db7u;
    for(unsigned i=0;i<30000;i++){
        uint8_t p[112];memcpy(p,t.pkg,sizeof(p));
        seed=seed*1664525u+1013904223u;
        p[(seed>>8)%sizeof(p)]^=(uint8_t)((seed>>24)|1u);
        const uint8_t *img=0;size_t len=0;
        int valid=spmp_db_verify_package(p,sizeof(p),0x1234,&img,&len);
        if(valid){assert(img==p+48 && len==64);}
        else {assert(img==0 && len==0);}
    }
    printf("dualboot: 14 scenarios + 112 truncated frames + 30000 mutations + CRC vector OK\n");
    return 0;
}
