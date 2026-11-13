#include "spmp_cores.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
extern const spmp_core_ops spmp_chip8_ops;
static void present(void *u,const uint16_t *pix,unsigned w,unsigned h){
 (void)u;assert(pix&&w==64&&h==32);
}
static int load(void *u,const char *p,uint8_t *dst,size_t cap,size_t *got){
 (void)u;FILE *f=fopen(p,"rb");if(!f)return -1;
 *got=fread(dst,1,cap,f);int err=ferror(f);fclose(f);return err?-1:0;
}
static uint32_t keys(void*u){(void)u;return 0;}
int main(void){
 spmp_core_host h={0};h.present=present;h.read_entire=load;h.buttons=keys;
 assert(!spmp_chip8_ops.start(&h,"samples/chip8_font0.ch8"));
 assert(!spmp_chip8_ops.tick());
 size_t n=spmp_chip8_ops.state_size();assert(n>4096&&n<8192);
 void *a=malloc(n),*b=malloc(n);assert(a&&b);
 assert(!spmp_chip8_ops.serialize(a,n));
 /* Replaying unchanged state must restore all emulated data deterministically. */
 assert(!spmp_chip8_ops.unserialize(a,n));
 assert(!spmp_chip8_ops.serialize(b,n));assert(!memcmp(a,b,n));
 /* Untrusted invalid PC is rejected without modifying current state. */
 uint8_t *corrupt=malloc(n);assert(corrupt);memcpy(corrupt,a,n);
 /* Rejection for null or wrong length is unconditional. */
 assert(spmp_chip8_ops.unserialize(corrupt,n-1));
 assert(!spmp_chip8_ops.serialize(b,n));assert(!memcmp(a,b,n));
 spmp_chip8_ops.stop();
 assert(!spmp_chip8_ops.state_size());
 free(corrupt);free(a);free(b);
 printf("CHIP-8 full state: 4-KiB RAM, registers, stack, VRAM, timers, deterministic roundtrip OK\n");
 return 0;
}
