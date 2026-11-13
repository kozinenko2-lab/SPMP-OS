#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../vendor/psx4all-dingoo-master/src/spmp_port/spmp_psx_opt.h"
#define W 1024u
#define H 512u
#define N (W*H)
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"FAIL at %d: %s\n",__LINE__,#x);exit(1);} tests++; } while(0)
static uint32_t rng=0xf007beef;
static uint32_t rnd(void) { rng^=rng<<13; rng^=rng>>17; rng^=rng<<5;return rng; }
static uint16_t a[N],b[N],scratch[N];
static int tests;
static int cache_calls;
static uintptr_t cache_begin,cache_end;
static void cache_test(uintptr_t p,uintptr_t q,void *ctx) {
    if (ctx != &tests) abort();
    cache_calls++; cache_begin=p; cache_end=q;
}
static void fill(void) { for(size_t i=0;i<N;i++)a[i]=(uint16_t)rnd(); memcpy(b,a,sizeof(a)); }
/* Independent snapshot reference covers overlap in both directions. */
static void reference_copy(int sx,int sy,int dx,int dy,unsigned ww,unsigned hh) {
   for(unsigned r=0;r<hh;r++)memcpy(scratch+r*ww,a+(sy+(int)r)*W+sx,ww*2);
   for(unsigned r=0;r<hh;r++)memcpy(b+(dy+(int)r)*W+dx,scratch+r*ww,ww*2);
}
static void reference_clear(int x,int y,unsigned ww,unsigned hh,uint16_t c) {
 for(unsigned r=0;r<hh;r++)for(unsigned k=0;k<ww;k++)b[(y+r)*W+x+k]=c;
}
static double now_sec(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1.e-9;}
static volatile uint16_t bench_sink;
static double fast_move_bench(unsigned n) {
    double t=now_sec();for(unsigned i=0;i<n;i++) {
       spmp_psx_gpu_move_16(a,W,H,40,20,48,30,640,220);
       bench_sink^=a[(i%200)*W+40];
    }return now_sec()-t;
}
static double scalar_move_bench(unsigned n) {
    double t=now_sec();for(unsigned i=0;i<n;i++) {
       /* original renderer loops from left to right, even for overlap */
       for(unsigned r=0;r<220;r++)for(unsigned k=0;k<640;k++)
         a[(30+r)*W+48+k]=a[(20+r)*W+40+k];
       bench_sink^=a[(i%200)*W+40];
    }return now_sec()-t;
}
static void test_move(void){
   const struct {int sx,sy,dx,dy;unsigned w,h;} v[]={{10,10,10,10,100,75},{100,100,102,100,100,80},{100,100,99,100,100,80},{100,100,100,101,150,100},{100,100,100,99,150,100},{0,0,0,0,1024,512},{0,0,13,20,500,400},{600,100,40,90,300,100},{0,0,900,500,124,12}};
   for(unsigned i=0;i<sizeof(v)/sizeof(v[0]);i++) {
      fill(); reference_copy(v[i].sx,v[i].sy,v[i].dx,v[i].dy,v[i].w,v[i].h);
      REQUIRE(spmp_psx_gpu_move_16(a,W,H,v[i].sx,v[i].sy,v[i].dx,v[i].dy,v[i].w,v[i].h)==1);
      REQUIRE(memcmp(a,b,sizeof(a))==0);
   }
   for(unsigned i=0;i<180;i++) {
      unsigned w=(rnd()%512)+1,h=(rnd()%200)+1;
      int sx=(int)(rnd()%(W-w+1)),sy=(int)(rnd()%(H-h+1));
      int dx=(int)(rnd()%(W-w+1)),dy=(int)(rnd()%(H-h+1));
      fill();reference_copy(sx,sy,dx,dy,w,h);
      REQUIRE(spmp_psx_gpu_move_16(a,W,H,sx,sy,dx,dy,w,h));
      REQUIRE(memcmp(a,b,sizeof(a))==0);
   }
   fill();REQUIRE(!spmp_psx_gpu_move_16(a,W,H,-1,2,3,4,20,4));REQUIRE(memcmp(a,b,sizeof(a))==0);
   REQUIRE(!spmp_psx_gpu_move_16(a,W,H,1000,200,999,10,100,2));
   REQUIRE(!spmp_psx_gpu_move_16(a,W,H,1,500,2,502,200,20));
}
static void test_clear(void){
  for(int i=0;i<150;i++){
     unsigned w=(rnd()%501)+1,h=(rnd()%155)+1;
     int x=(int)(rnd()%(W-w+1)),y=(int)(rnd()%(H-h+1));
     uint16_t color=i%3?((uint16_t)rnd()):0;
     fill();reference_clear(x,y,w,h,color);
     REQUIRE(spmp_psx_gpu_clear_16(a,W,H,x,y,w,h,color));
     REQUIRE(!memcmp(a,b,sizeof(a)));
  }
  fill(); REQUIRE(!spmp_psx_gpu_clear_16(a,W,H,1000,500,40,20,0x7fff));REQUIRE(!memcmp(a,b,sizeof(a)));
}
static void test_cycles(void){
   REQUIRE(spmp_psx_scaled_cycles(0,2)==0);
   REQUIRE(spmp_psx_scaled_cycles(300,3)==900);
   REQUIRE(spmp_psx_scaled_cycles(0xffffffffu,2)==0xfffffffeu);
   for(unsigned i=0;i<100000;i++){
      uint32_t a=rnd(),b=rnd();uint32_t expected=(uint32_t)((uint64_t)a*b);
      REQUIRE(spmp_psx_scaled_cycles(a,b)==expected);
   }
}
static uint32_t dma_words[600];
static unsigned dma_n;
static void dma_sink(uint32_t word, void *u) {
    REQUIRE(u==&dma_n);
    if (dma_n>=600) abort();
    dma_words[dma_n++]=word;
}
static void test_dma(void) {
    uint32_t *ram=(uint32_t*)calloc(0x80000,sizeof(uint32_t));
    REQUIRE(ram!=NULL);
    ram[0x100>>2]=(2u<<24)|0x200u;
    ram[(0x100>>2)+1]=0x01020304;
    ram[(0x100>>2)+2]=0x05060708;
    ram[0x200>>2]=(1u<<24)|0x00ffffffu;
    ram[(0x200>>2)+1]=0x0a0b0c0d;
    dma_n=0;
    REQUIRE(spmp_psx_gpu_dma_walk(ram,0x200000,0x100,128,dma_sink,&dma_n)==2);
    REQUIRE(dma_n==3 && dma_words[0]==0x01020304u &&
            dma_words[1]==0x05060708u && dma_words[2]==0x0a0b0c0du);
    /* Command data crossing last physical RAM word wraps to physical 0. */
    ram[0x7ffff]=(2u<<24)|0x00ffffffu;
    ram[0]=0x11223344;ram[1]=0xaabbccdd;
    dma_n=0;
    REQUIRE(spmp_psx_gpu_dma_walk(ram,0x200000,0x1ffffc,128,dma_sink,&dma_n)==1);
    REQUIRE(dma_n==2 && dma_words[0]==0x11223344 && dma_words[1]==0xaabbccdd);
    /* Bad GPU chain cannot spin forever. */
    ram[0x100>>2]=(0u<<24)|0x100u;
    dma_n=0;
    REQUIRE(spmp_psx_gpu_dma_walk(ram,0x200000,0x100,4,dma_sink,&dma_n)==-1);
    REQUIRE(dma_n==0);
    REQUIRE(spmp_psx_gpu_dma_walk(ram,0x100000,0x100,4,dma_sink,&dma_n)==-1);
    free(ram);
}
static void test_cd_cache(void) {
    REQUIRE(!spmp_psx_cd_cache_hit(50,50,0));
    REQUIRE(spmp_psx_cd_cache_hit(50,50,1));
    REQUIRE(!spmp_psx_cd_cache_hit(51,50,1));
    REQUIRE(spmp_psx_cd_cache_hit(51,50,2));
    REQUIRE(!spmp_psx_cd_cache_hit(49,50,2));
    REQUIRE(!spmp_psx_cd_cache_hit(0,0x7fffffff,16));
    REQUIRE(!spmp_psx_cd_cache_hit(0,0xfffffff0u,32));
}
static void test_cache(void){
   REQUIRE(!spmp_psx_icache_ready());
   REQUIRE(!spmp_psx_bind_icache(NULL,NULL));
   spmp_psx_icache_sync(12,20,0); REQUIRE(cache_calls==0);
   REQUIRE(spmp_psx_bind_icache(cache_test,&tests));
   REQUIRE(spmp_psx_icache_ready());
   spmp_psx_icache_sync(0x4000,0x5000,0);
   REQUIRE(cache_calls==1 && cache_begin==0x4000 && cache_end==0x5000);
   spmp_psx_icache_sync(0x5000,0x4000,0);REQUIRE(cache_calls==1);
   spmp_psx_unbind_icache(); REQUIRE(!spmp_psx_icache_ready());
   spmp_psx_icache_sync(0x4000,0x5000,0); REQUIRE(cache_calls==1);
}
int main(int argc,char **argv){
    test_move();test_clear();test_cycles();test_cd_cache();test_dma();test_cache();
    printf("PSX4ALL SPMP-OS: %d checks passed\n",tests);
    if(argc>1 && !strcmp(argv[1],"--bench")){
      fill(); double x=fast_move_bench(300),y=scalar_move_bench(300);
      printf("host VRAM move (300x 640x220 pixels): memmove rows %.6fs, scalar legacy %.6fs, ratio %.2fx (NOT console FPS)\n",x,y,y/x);
    }
    return 0;
}
