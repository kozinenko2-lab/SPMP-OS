#include "gpsp_heap_bridge.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static _Alignas(16) unsigned char ram[1024*1024];
int main(void) {
    assert(gpsp_heap_malloc(10)==NULL);
    for(int n=0;n<100;n++) {
        assert(gpsp_heap_bind(ram,sizeof(ram))==0);
        void *p=gpsp_heap_malloc(100);
        assert(p); memset(p,0xa5,100);
        assert(gpsp_heap_unbind()!=0); /* Cannot unload allocated module. */
        void *q=gpsp_heap_realloc(p,240);
        assert(q); for(int i=0;i<100;i++)assert(((unsigned char*)q)[i]==0xa5);
        void *z=gpsp_heap_calloc(16,40);
        assert(z);for(int i=0;i<640;i++)assert(!((unsigned char*)z)[i]);
        assert(gpsp_heap_calloc(SIZE_MAX,2)==NULL);
        assert(gpsp_heap_malloc(SIZE_MAX)==NULL);
        gpsp_heap_free(z);gpsp_heap_free(q);
        assert(gpsp_heap_unbind()==0);
        assert(gpsp_heap_errors()==0);
    }
    puts("gpSP heap bridge: 100 bind/alloc/realloc/free/unbind cycles OK");
}
