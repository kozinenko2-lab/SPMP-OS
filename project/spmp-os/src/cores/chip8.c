/* SPMP-OS original CHIP-8 interpreter (no eCos dependencies).
 * Content is a local .ch8/.c8 ROM, max 3584 bytes, loaded at 0x200.
 * Baseline CHIP-8 opcodes, 64x32 monochrome. Not SCHIP or XO-CHIP.
 * All memory/PC/stack/font accesses bounds-checked. Host preview proven;
 * device audio/keys integration remains future work.
 */
#include "spmp_cores.h"
#include <stdint.h>
#include <string.h>

#define CW 64u
#define CH 32u
#define ROM_START 0x200u
#define MEM_CAP 4096u
static const uint8_t font[80]={
 0xf0,0x90,0x90,0x90,0xf0, 0x20,0x60,0x20,0x20,0x70,
 0xf0,0x10,0xf0,0x80,0xf0, 0xf0,0x10,0xf0,0x10,0xf0,
 0x90,0x90,0xf0,0x10,0x10, 0xf0,0x80,0xf0,0x10,0xf0,
 0xf0,0x80,0xf0,0x90,0xf0, 0xf0,0x10,0x20,0x40,0x40,
 0xf0,0x90,0xf0,0x90,0xf0, 0xf0,0x90,0xf0,0x10,0xf0,
 0xf0,0x90,0xf0,0x90,0x90, 0xe0,0x90,0xe0,0x90,0xe0,
 0xf0,0x80,0x80,0x80,0xf0, 0xe0,0x90,0x90,0x90,0xe0,
 0xf0,0x80,0xf0,0x80,0xf0, 0xf0,0x80,0xf0,0x80,0x80
};
typedef struct {
 uint8_t mem[MEM_CAP],v[16],keys[16],delay,sound,sp,screen[CW*CH];
 uint16_t stack[16],i,pc,pixels[CW*CH];
 uint32_t rng;
 const spmp_core_host *host;
 int live;
} chip8_vm;
static chip8_vm vm;
static uint8_t rnd(void){
 vm.rng ^= vm.rng<<13;vm.rng ^= vm.rng>>17;vm.rng ^= vm.rng<<5;
 return (uint8_t)(vm.rng>>24);
}
static int allowed(unsigned address,unsigned count){
 return address < MEM_CAP && count <= MEM_CAP-address;
}
static int execute_one(void){
 unsigned op,x,y,n,nn,nnn;
 if(!allowed(vm.pc,2))return -1;
 op=((unsigned)vm.mem[vm.pc]<<8)|vm.mem[vm.pc+1];vm.pc+=2;
 x=(op>>8)&15;y=(op>>4)&15;n=op&15;nn=op&255;nnn=op&4095;
 switch(op>>12){
 case 0:
   if(op==0x00e0){for(unsigned pixel=0;pixel<CW*CH;pixel++)vm.screen[pixel]=0;return 0;}
   if(op==0x00ee){if(vm.sp==0)return -1;vm.pc=vm.stack[--vm.sp];return 0;}
   return -1;
 case 1:vm.pc=(uint16_t)nnn;return 0;
 case 2:if(vm.sp==16)return -1;vm.stack[vm.sp++]=vm.pc;vm.pc=(uint16_t)nnn;return 0;
 case 3:if(vm.v[x]==nn)vm.pc+=2;return 0;
 case 4:if(vm.v[x]!=nn)vm.pc+=2;return 0;
 case 5:if(n)return -1;if(vm.v[x]==vm.v[y])vm.pc+=2;return 0;
 case 6:vm.v[x]=(uint8_t)nn;return 0;
 case 7:vm.v[x]=(uint8_t)(vm.v[x]+nn);return 0;
 case 8:{unsigned a=vm.v[x],b=vm.v[y];
  switch(n){
   case 0:vm.v[x]=(uint8_t)b;break;
   case 1:vm.v[x]=(uint8_t)(a|b);vm.v[15]=0;break;
   case 2:vm.v[x]=(uint8_t)(a&b);vm.v[15]=0;break;
   case 3:vm.v[x]=(uint8_t)(a^b);vm.v[15]=0;break;
   case 4:vm.v[x]=(uint8_t)(a+b);vm.v[15]=(uint8_t)((a+b)>255);break;
   case 5:vm.v[x]=(uint8_t)(a-b);vm.v[15]=(uint8_t)(a>=b);break;
   case 6:vm.v[x]=(uint8_t)(a>>1);vm.v[15]=(uint8_t)(a&1);break;
   case 7:vm.v[x]=(uint8_t)(b-a);vm.v[15]=(uint8_t)(b>=a);break;
   case 14:vm.v[x]=(uint8_t)(a<<1);vm.v[15]=(uint8_t)((a>>7)&1);break;
   default:return -1;
  }return 0;}
 case 9:if(n)return -1;if(vm.v[x]!=vm.v[y])vm.pc+=2;return 0;
 case 10:vm.i=(uint16_t)nnn;return 0;
 case 11:vm.pc=(uint16_t)(nnn+vm.v[0]);return 0;
 case 12:vm.v[x]=(uint8_t)(rnd()&nn);return 0;
 case 13:{unsigned sx=vm.v[x]%CW,sy=vm.v[y]%CH;
   if(!allowed(vm.i,n))return -1;
   vm.v[15]=0;
   for(unsigned row=0;row<n;row++){
    unsigned yy=sy+row;if(yy>=CH)continue;
    unsigned bits=vm.mem[vm.i+row];
    for(unsigned bit=0;bit<8;bit++){
      unsigned xx=sx+bit;if(xx>=CW)continue;
      unsigned at=yy*CW+xx;
      if((bits>>(7-bit))&1){if(vm.screen[at])vm.v[15]=1;vm.screen[at]^=1;}
    }
   }return 0;}
 case 14:
   if(n==0x0e){if(vm.keys[vm.v[x]&15])vm.pc+=2;return 0;}
   if(n==0x01){if(!vm.keys[vm.v[x]&15])vm.pc+=2;return 0;}
   return -1;
 case 15:
   switch(nn){
   case 0x07:vm.v[x]=vm.delay;return 0;
   case 0x0a:
     for(unsigned k=0;k<16;k++)if(vm.keys[k]){vm.v[x]=(uint8_t)k;return 0;}
     vm.pc-=2;return 0;
   case 0x15:vm.delay=vm.v[x];return 0;
   case 0x18:vm.sound=vm.v[x];return 0;
   case 0x1e:vm.i=(uint16_t)(vm.i+vm.v[x]);return 0;
   case 0x29:vm.i=(uint16_t)((vm.v[x]&15)*5);return 0;
   case 0x33:if(!allowed(vm.i,3))return -1;
     vm.mem[vm.i]=(uint8_t)(vm.v[x]/100);
     vm.mem[vm.i+1]=(uint8_t)((vm.v[x]/10)%10);
     vm.mem[vm.i+2]=(uint8_t)(vm.v[x]%10);return 0;
   case 0x55:if(!allowed(vm.i,x+1))return -1;
     for(unsigned k=0;k<=x;k++){vm.mem[vm.i+k]=vm.v[k];}return 0;
   case 0x65:if(!allowed(vm.i,x+1))return -1;
     for(unsigned k=0;k<=x;k++){vm.v[k]=vm.mem[vm.i+k];}return 0;
   default:return -1;
   }
 default:return -1;
 }
}
static int c8_start(const spmp_core_host *host,const char *path){
 if(!host||!host->present||!host->read_entire||!path)return -1;
 /* BSS is static; reset deterministically without a libc dependency. */
 uint8_t *bytes=(uint8_t *)&vm;
 for(size_t k=0;k<sizeof vm;k++)bytes[k]=0;
 for(size_t k=0;k<sizeof font;k++)vm.mem[k]=font[k];
 size_t got=0;
 if(host->read_entire(host->user,path,vm.mem+ROM_START,
                      MEM_CAP-ROM_START,&got)||got<2||(got&1))return -1;
 vm.pc=ROM_START;vm.rng=0x12345678u;vm.host=host;vm.live=1;
 return 0;
}
static int c8_tick(void){
 if(!vm.live)return -1;
 uint32_t key_bits=vm.host->buttons?vm.host->buttons(vm.host->user):0;
 for(unsigned k=0;k<16;k++)vm.keys[k]=(uint8_t)((key_bits>>k)&1u);
 for(unsigned count=0;count<10;count++)if(execute_one())return -1;
 if(vm.delay)vm.delay--;
 if(vm.sound)vm.sound--;
 for(unsigned p=0;p<CW*CH;p++)vm.pixels[p]=vm.screen[p]?0xffff:0x0000;
 vm.host->present(vm.host->user,vm.pixels,CW,CH);
 return 0;
}
static void c8_stop(void){vm.live=0;vm.host=0;}
/* Stable state excluding host pointers and ephemeral rendered pixels. */
typedef struct {
 uint8_t mem[MEM_CAP],v[16],keys[16],delay,sound,sp,screen[CW*CH];
 uint16_t stack[16],i,pc;
 uint32_t rng;
} chip8_state;
static size_t c8_state_size(void){return vm.live?sizeof(chip8_state):0;}
static int c8_serialize(void *dst,size_t len){
 if(!vm.live||!dst||len!=sizeof(chip8_state))return -1;
 chip8_state st={0};
 memcpy(st.mem,vm.mem,sizeof st.mem);memcpy(st.v,vm.v,sizeof st.v);
 memcpy(st.keys,vm.keys,sizeof st.keys);
 st.delay=vm.delay;st.sound=vm.sound;st.sp=vm.sp;
 memcpy(st.screen,vm.screen,sizeof st.screen);
 memcpy(st.stack,vm.stack,sizeof st.stack);
 st.i=vm.i;st.pc=vm.pc;st.rng=vm.rng;
 memcpy(dst,&st,sizeof st);return 0;
}
static int c8_unserialize(const void *src,size_t len){
 if(!vm.live||!src||len!=sizeof(chip8_state))return -1;
 chip8_state st;memcpy(&st,src,sizeof st);
 if(st.sp>16||st.pc>=MEM_CAP||st.i>=MEM_CAP)return -1;
 for(unsigned k=0;k<16;k++)if(st.stack[k]>=MEM_CAP)return -1;
 memcpy(vm.mem,st.mem,sizeof st.mem);memcpy(vm.v,st.v,sizeof st.v);
 memcpy(vm.keys,st.keys,sizeof st.keys);
 vm.delay=st.delay;vm.sound=st.sound;vm.sp=st.sp;
 memcpy(vm.screen,st.screen,sizeof st.screen);
 memcpy(vm.stack,st.stack,sizeof st.stack);
 vm.i=st.i;vm.pc=st.pc;vm.rng=st.rng;
 return 0;
}
const spmp_core_ops spmp_chip8_ops={c8_start,c8_tick,c8_stop,c8_state_size,c8_serialize,c8_unserialize,NULL};
