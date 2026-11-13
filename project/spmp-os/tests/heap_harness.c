#include "spmp_heap.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#define REQUIRE(x) do {if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static unsigned char ram[128*1024+64];
int main(void){
    spmp_heap h={0};
    REQUIRE(spmp_heap_init(&h,ram+3,sizeof(ram)-6)==0);
    REQUIRE(spmp_heap_check(&h)==0);
    size_t initial=spmp_heap_free_bytes(&h);
    void *p[200]={0}; size_t sizes[200]={0};
    uint32_t seed=0x123abc45u;
    size_t operations=0;
    for(int round=0;round<80;round++){
      for(int i=0;i<200;i++){
        seed=seed*1664525u+1013904223u;
        unsigned action=seed%4;
        if(p[i] && action==0){
            unsigned char *b=p[i];for(size_t j=0;j<sizes[i];j++)REQUIRE(b[j]==(unsigned char)i);
            REQUIRE(spmp_heap_free(&h,p[i])==0);p[i]=0;sizes[i]=0;
        }else if(p[i] && action==1){
            size_t n=(seed%800u)+1u;
            unsigned char *b=spmp_heap_realloc(&h,p[i],n);
            if(b){
                size_t check=sizes[i]<n?sizes[i]:n;
                for(size_t j=0;j<check;j++)REQUIRE(b[j]==(unsigned char)i);
                memset(b,i,n);p[i]=b;sizes[i]=n;
            }
        }else if(!p[i]){
            size_t n=(seed%500u)+1u;
            unsigned char *b=spmp_heap_alloc(&h,n);
            if(b){REQUIRE((uintptr_t)b%_Alignof(max_align_t)==0);memset(b,i,n);p[i]=b;sizes[i]=n;}
        }
        REQUIRE(spmp_heap_check(&h)==0);operations++;
      }
    }
    for(int i=0;i<200;i++)if(p[i]) {REQUIRE(spmp_heap_free(&h,p[i])==0);p[i]=0;}
    REQUIRE(spmp_heap_free_bytes(&h)==initial);
    REQUIRE(spmp_heap_calloc(&h,SIZE_MAX,4)==0);
    REQUIRE(spmp_heap_alloc(&h,SIZE_MAX)==0);
    unsigned char *x=spmp_heap_calloc(&h,8,64);
    REQUIRE(x!=0);
    for(size_t j=0;j<512;j++)REQUIRE(x[j]==0);
    REQUIRE(spmp_heap_free(&h,x)==0);
    REQUIRE(spmp_heap_free(&h,x)!=0);
    REQUIRE(spmp_heap_free_bytes(&h)==initial);
    unsigned char alien=0;REQUIRE(spmp_heap_free(&h,&alien)!=0);
    REQUIRE(spmp_heap_init(&h,ram+3,sizeof(ram)-6)==0);
    REQUIRE(spmp_heap_free_bytes(&h)==initial);
    printf("PASS %zu random heap operations, coalescing, overflow, alignment and double-free checks\n",operations);
    return 0;
}
