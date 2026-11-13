/* SPMP-OS preliminary NES emulator: self-contained iNES1 mapper 0/2/3,
 * NMOS 6502, tile + sprite PPU, serial joypad, battery backed PRG RAM.
 * This is an intentionally constrained compatibility baseline, not FCEUmm.
 * No eCos, RedBoot, host libc or external ROM assets are required.
 * TODO: cycle-timed PPU/APU, MMC1/MMC3 and remaining undocumented CPU ops.
 * Copyright (c) 2026 SPMP-OS contributors. MIT license, see LICENSE-SPMP.
 */
#include "spmp_cores.h"
#include <stdint.h>
#include <stddef.h>

#define PRG_CAP (512u*1024u)
#define CHR_CAP (256u*1024u)
#define ROM_CAP (PRG_CAP+CHR_CAP+16u+512u)
#define W 256u
#define H 240u
#define FLAG_C 1u
#define FLAG_Z 2u
#define FLAG_I 4u
#define FLAG_D 8u
#define FLAG_B 16u
#define FLAG_U 32u
#define FLAG_V 64u
#define FLAG_N 128u

typedef struct {
 uint8_t image[ROM_CAP];
 uint8_t chr_ram[8192], cpu_ram[2048], sram[8192], nt[4096], oam[256], pal[32];
 uint16_t frame[W*H];
 uint32_t prg_bytes, chr_bytes;
 uint8_t mapper, mirror_four, mirror_vertical, chr_bank, prg_bank;
 uint8_t ppuctrl,ppumask,ppustatus,oamaddr,ppudata_buf,ppu_latch,scroll_x,scroll_y;
 uint16_t ppuaddr,pputemp;
 uint8_t joy_shift[2], joy_strobe;
 uint8_t A,X,Y,S,P, live,dirty, nmi,battery;
 uint16_t pc;
 uint64_t cycles;
 const spmp_core_host *host;
 const char *content;
} nes_vm;
static nes_vm vm;
static void zero(void *ptr,size_t n){uint8_t *p=ptr;for(size_t i=0;i<n;i++)p[i]=0;}
static void fill(void *ptr,uint8_t b,size_t n){uint8_t *p=ptr;for(size_t i=0;i<n;i++)p[i]=b;}
static uint8_t *chr_ptr(void){return vm.chr_bytes?vm.image+16u+vm.prg_bytes:vm.chr_ram;}
static uint8_t prg_get(uint16_t a){
 if(!vm.prg_bytes)return 0xff;
 uint32_t ofs=(uint32_t)a-0x8000u;
 if(vm.mapper==2 && a<0xc000)ofs=(uint32_t)vm.prg_bank*0x4000u+(ofs&0x3fffu);
 else if(vm.mapper==2)ofs=vm.prg_bytes-0x4000u+(ofs&0x3fffu);
 else ofs %=vm.prg_bytes;
 if(ofs>=vm.prg_bytes)return 0xff;
 return vm.image[16u+ofs];
}
static uint16_t nt_address(uint16_t a){
 uint16_t t=(uint16_t)((a-0x2000u)&0xfffu);
 if(vm.mirror_four)return t;
 unsigned page=(t>>10)&3u;
 page=vm.mirror_vertical?(page&1u):(page>>1u);
 return (uint16_t)(page*1024u+(t&1023u));
}
static uint8_t ppu_read(uint16_t a){
 a&=0x3fffu;
 if(a<0x2000){
  uint32_t ofs=(uint32_t)(vm.mapper==3 ? vm.chr_bank : 0u)*0x2000u+a;
  if(vm.chr_bytes)ofs%=vm.chr_bytes; else ofs=a;
  return chr_ptr()[ofs];
 }
 if(a<0x3f00)return vm.nt[nt_address(a)];
 unsigned p=(unsigned)(a&0x1fu);
 if((p&0x13u)==0x10u)p-=0x10u;
 return vm.pal[p];
}
static void ppu_write(uint16_t a,uint8_t v){
 a&=0x3fff;
 if(a<0x2000){if(!vm.chr_bytes)vm.chr_ram[a]=v;return;}
 if(a<0x3f00){vm.nt[nt_address(a)]=v;return;}
 unsigned p=a&31u;
 if((p&0x13u)==0x10u)p-=0x10u;
 vm.pal[p]=v&63u;
}
static uint8_t cpu_read(uint16_t a){
 if(a<0x2000)return vm.cpu_ram[a&0x7ffu];
 if(a<0x4000){
  uint8_t reg=(uint8_t)(a&7u);
  if(reg==2){uint8_t v=(uint8_t)(vm.ppustatus|0x1fu);vm.ppustatus&=0x7fu;vm.ppu_latch=0;return v;}
  if(reg==4)return vm.oam[vm.oamaddr];
  if(reg==7){uint8_t out=vm.ppudata_buf;vm.ppudata_buf=ppu_read(vm.ppuaddr);
   if((vm.ppuaddr&0x3fff)>=0x3f00)out=ppu_read(vm.ppuaddr);
   vm.ppuaddr+=(vm.ppuctrl&4u)?32u:1u;return out;}
  return 0;
 }
 if(a==0x4016||a==0x4017){unsigned j=a&1u;uint8_t out=(uint8_t)((vm.joy_shift[j]&1u)|0x40u);
  if(!vm.joy_strobe)vm.joy_shift[j]=(uint8_t)((vm.joy_shift[j]>>1u)|0x80u);
  return out;}
 if(a<0x6000)return 0;
 if(a<0x8000)return vm.sram[a&0x1fffu];
 return prg_get(a);
}
static void cpu_write(uint16_t a,uint8_t v){
 if(a<0x2000){vm.cpu_ram[a&0x7ffu]=v;return;}
 if(a<0x4000){
  switch(a&7u){
   case 0:vm.ppuctrl=v;break;
   case 1:vm.ppumask=v;break;
   case 3:vm.oamaddr=v;break;
   case 4:vm.oam[vm.oamaddr++]=v;break;
   case 5:if(!vm.ppu_latch){vm.scroll_x=v;vm.ppu_latch=1;}else{vm.scroll_y=v;vm.ppu_latch=0;}break;
   case 6:if(!vm.ppu_latch){vm.pputemp=(uint16_t)((v&63u)<<8);vm.ppu_latch=1;}
          else{vm.ppuaddr=(uint16_t)(vm.pputemp|v);vm.ppu_latch=0;}break;
   case 7:ppu_write(vm.ppuaddr,v);vm.ppuaddr+=(vm.ppuctrl&4u)?32u:1u;break;
  }return;
 }
 if(a==0x4014){unsigned base=(unsigned)v<<8;for(unsigned i=0;i<256;i++)vm.oam[vm.oamaddr++]=cpu_read((uint16_t)(base+i));vm.cycles+=513u;return;}
 if(a==0x4016){vm.joy_strobe=v&1u;if(vm.joy_strobe){
  uint32_t keys=vm.host->buttons?vm.host->buttons(vm.host->user):0;
  /* emulator bits: 0 up, 1 down, 2 left, 3 right, 4 B, 5 A, 6 start, 7 select */
  vm.joy_shift[0]=(uint8_t)(((keys>>5)&1u)|(((keys>>4)&1u)<<1)|(((keys>>7)&1u)<<2)|(((keys>>6)&1u)<<3)|(((keys>>0)&1u)<<4)|(((keys>>1)&1u)<<5)|(((keys>>2)&1u)<<6)|(((keys>>3)&1u)<<7));
 }return;}
 if(a<0x6000)return; /* APU not yet emulated */
 if(a<0x8000){if(vm.sram[a&0x1fff]!=v){vm.sram[a&0x1fff]=v;vm.dirty=1;}return;}
 if(vm.mapper==2){vm.prg_bank=(uint8_t)(v%(vm.prg_bytes/0x4000u));return;}
 if(vm.mapper==3){vm.chr_bank=(uint8_t)(v%(vm.chr_bytes/0x2000u));return;}
}
static uint8_t rdpc(void){uint8_t x=cpu_read(vm.pc);vm.pc++;return x;}
static uint16_t read16(uint16_t addr){unsigned lo=cpu_read(addr),hi=cpu_read((uint16_t)(addr+1));return (uint16_t)(lo|(hi<<8));}
static uint16_t fetch16(void){unsigned lo=rdpc(),hi=rdpc();return (uint16_t)(lo|(hi<<8));}
static void push(uint8_t v){cpu_write((uint16_t)(0x100u+vm.S),v);vm.S--;}
static uint8_t pop(void){vm.S++;return cpu_read((uint16_t)(0x100u+vm.S));}
static void zn(uint8_t v){vm.P=(uint8_t)((vm.P&~(FLAG_Z|FLAG_N)) | (v?0:FLAG_Z) | (v&FLAG_N));}
static void compare(uint8_t lhs,uint8_t rhs){vm.P=(uint8_t)((vm.P&~FLAG_C)|(lhs>=rhs));zn((uint8_t)(lhs-rhs));}
static void adc(uint8_t v){unsigned s=(unsigned)vm.A+v+(vm.P&FLAG_C);uint8_t out=(uint8_t)s;
 vm.P=(uint8_t)((vm.P&~(FLAG_C|FLAG_V)) | (s>255u?FLAG_C:0u) | (((~(vm.A^v)&(vm.A^out))&0x80u)?FLAG_V:0u));vm.A=out;zn(out);
}
static uint16_t address_mode(unsigned op,uint8_t *imm){
 /* 6502 opcode groups 01 and 11 have a regular addressing pattern. */
 unsigned mode=(op>>2)&7u;
 if((op&3u)==1u){
  switch(mode){
   case 0:{uint8_t zp=(uint8_t)(rdpc()+vm.X);return (uint16_t)(cpu_read(zp)|((unsigned)cpu_read((uint8_t)(zp+1))<<8));}
   case 1:return rdpc();
   case 2:*imm=1;return vm.pc++;
   case 3:return fetch16();
   case 4:{uint8_t zp=rdpc();uint16_t base=(uint16_t)(cpu_read(zp)|((unsigned)cpu_read((uint8_t)(zp+1))<<8));return (uint16_t)(base+vm.Y);}
   case 5:return (uint8_t)(rdpc()+vm.X);
   case 6:return (uint16_t)(fetch16()+vm.Y);
   case 7:return (uint16_t)(fetch16()+vm.X);
  }
 }
 return 0;
}
static void branch(unsigned enabled){int8_t offset=(int8_t)rdpc();if(enabled)vm.pc=(uint16_t)(vm.pc+offset);}
static void interrupt(uint16_t vector,uint8_t brk){push((uint8_t)(vm.pc>>8));push((uint8_t)vm.pc);
 push((uint8_t)((vm.P&~FLAG_B)|(brk?FLAG_B:0u)|FLAG_U));vm.P|=FLAG_I;vm.pc=read16(vector);}
static int cpu_step(void){
 if(vm.nmi){vm.nmi=0;interrupt(0xfffa,0);vm.cycles+=7;}
 uint8_t op=rdpc(),v=0,imm=0;uint16_t a=0;
 vm.cycles+=2; /* approximation: no instruction-level PPU timing yet */
 if((op&3u)==1u){
  unsigned kind=(unsigned)op>>5;
  if(kind==4 && ((op>>2)&7u)==2u)return -1; /* STA #illegal */
  a=address_mode(op,&imm);
  /* Writes must not read side-effectful memory-mapped I/O first. */
  if(kind!=4)v=cpu_read(a);
  switch(kind){
   case 0:vm.A=(uint8_t)(vm.A|v);zn(vm.A);break;
   case 1:vm.A=(uint8_t)(vm.A&v);zn(vm.A);break;
   case 2:vm.A=(uint8_t)(vm.A^v);zn(vm.A);break;
   case 3:adc(v);break;
   case 4:cpu_write(a,vm.A);break;
   case 5:vm.A=v;zn(vm.A);break;
   case 6:compare(vm.A,v);break;
   case 7:adc((uint8_t)~v);break;
  }
  vm.cycles+=2;return 0;
 }
 switch(op){
 /* Interrupts, calls, returns and branch conditions. */
 case 0x00:vm.pc++;interrupt(0xfffe,1);break;
 case 0x20:a=fetch16();push((uint8_t)((vm.pc-1)>>8));push((uint8_t)(vm.pc-1));vm.pc=a;break;
 case 0x40:vm.P=(uint8_t)((pop()|FLAG_U)&~FLAG_B);a=pop();vm.pc=(uint16_t)(a|((unsigned)pop()<<8));break;
 case 0x60:a=pop();vm.pc=(uint16_t)((a|((unsigned)pop()<<8))+1);break;
 case 0x4c:vm.pc=fetch16();break;
 case 0x6c:a=fetch16();vm.pc=(uint16_t)(cpu_read(a)|((unsigned)cpu_read((uint16_t)((a&0xff00u)|((a+1u)&255u)))<<8));break;
 case 0x10:branch(!(vm.P&FLAG_N));break;
 case 0x30:branch(vm.P&FLAG_N);break;
 case 0x50:branch(!(vm.P&FLAG_V));break;
 case 0x70:branch(vm.P&FLAG_V);break;
 case 0x90:branch(!(vm.P&FLAG_C));break;
 case 0xb0:branch(vm.P&FLAG_C);break;
 case 0xd0:branch(!(vm.P&FLAG_Z));break;
 case 0xf0:branch(vm.P&FLAG_Z);break;
 /* Stack, transfers, register updates */
 case 0x08:push((uint8_t)(vm.P|FLAG_B|FLAG_U));break;
 case 0x28:vm.P=(uint8_t)((pop()|FLAG_U)&~FLAG_B);break;
 case 0x48:push(vm.A);break;
 case 0x68:vm.A=pop();zn(vm.A);break;
 case 0xaa:vm.X=vm.A;zn(vm.X);break;
 case 0x8a:vm.A=vm.X;zn(vm.A);break;
 case 0xa8:vm.Y=vm.A;zn(vm.Y);break;
 case 0x98:vm.A=vm.Y;zn(vm.A);break;
 case 0xba:vm.X=vm.S;zn(vm.X);break;
 case 0x9a:vm.S=vm.X;break;
 case 0xe8:vm.X++;zn(vm.X);break;
 case 0xca:vm.X--;zn(vm.X);break;
 case 0xc8:vm.Y++;zn(vm.Y);break;
 case 0x88:vm.Y--;zn(vm.Y);break;
 case 0x18:vm.P&=(uint8_t)~FLAG_C;break;
 case 0x38:vm.P|=FLAG_C;break;
 case 0x58:vm.P&=(uint8_t)~FLAG_I;break;
 case 0x78:vm.P|=FLAG_I;break;
 case 0xb8:vm.P&=(uint8_t)~FLAG_V;break;
 case 0xd8:vm.P&=(uint8_t)~FLAG_D;break;
 case 0xf8:vm.P|=FLAG_D;break;
 case 0xea:break;
 /* BIT */
 case 0x24:a=rdpc();goto bit_op;
 case 0x2c:a=fetch16();bit_op:v=cpu_read(a);vm.P=(uint8_t)((vm.P&~(FLAG_Z|FLAG_N|FLAG_V)) | ((vm.A&v)?0:FLAG_Z) | (v&(FLAG_N|FLAG_V)));break;
 /* STX and STY. */
 case 0x86:a=rdpc();goto stx;case 0x96:a=(uint8_t)(rdpc()+vm.Y);goto stx;case 0x8e:a=fetch16();stx:cpu_write(a,vm.X);break;
 case 0x84:a=rdpc();goto sty;case 0x94:a=(uint8_t)(rdpc()+vm.X);goto sty;case 0x8c:a=fetch16();sty:cpu_write(a,vm.Y);break;
 /* LDX and LDY. */
 case 0xa2:a=vm.pc++;goto ldx;case 0xa6:a=rdpc();goto ldx;case 0xb6:a=(uint8_t)(rdpc()+vm.Y);goto ldx;case 0xae:a=fetch16();goto ldx;case 0xbe:a=(uint16_t)(fetch16()+vm.Y);ldx:vm.X=cpu_read(a);zn(vm.X);break;
 case 0xa0:a=vm.pc++;goto ldy;case 0xa4:a=rdpc();goto ldy;case 0xb4:a=(uint8_t)(rdpc()+vm.X);goto ldy;case 0xac:a=fetch16();goto ldy;case 0xbc:a=(uint16_t)(fetch16()+vm.X);ldy:vm.Y=cpu_read(a);zn(vm.Y);break;
 /* CPX and CPY. */
 case 0xe0:v=rdpc();goto cpx;case 0xe4:v=cpu_read(rdpc());goto cpx;case 0xec:v=cpu_read(fetch16());cpx:compare(vm.X,v);break;
 case 0xc0:v=rdpc();goto cpy;case 0xc4:v=cpu_read(rdpc());goto cpy;case 0xcc:v=cpu_read(fetch16());cpy:compare(vm.Y,v);break;
 /* INC, DEC and register shifts: 6502 group-10. */
 case 0xe6:case 0xc6:case 0x06:case 0x26:case 0x46:case 0x66:a=rdpc();goto rmw;
 case 0xf6:case 0xd6:case 0x16:case 0x36:case 0x56:case 0x76:a=(uint8_t)(rdpc()+vm.X);goto rmw;
 case 0xee:case 0xce:case 0x0e:case 0x2e:case 0x4e:case 0x6e:a=fetch16();goto rmw;
 case 0xfe:case 0xde:case 0x1e:case 0x3e:case 0x5e:case 0x7e:a=(uint16_t)(fetch16()+vm.X);goto rmw;
 case 0x0a:case 0x2a:case 0x4a:case 0x6a:imm=1;v=vm.A;goto shift;
 rmw:v=cpu_read(a);
 shift:switch(op&0xe0u){
  case 0xe0:v++;break;case 0xc0:v--;break;
  case 0x00:vm.P=(uint8_t)((vm.P&~FLAG_C)|(v>>7));v<<=1;break;
  case 0x20:{uint8_t old=vm.P&FLAG_C;vm.P=(uint8_t)((vm.P&~FLAG_C)|(v>>7));v=(uint8_t)((v<<1)|old);break;}
  case 0x40:vm.P=(uint8_t)((vm.P&~FLAG_C)|(v&1u));v>>=1;break;
  case 0x60:{uint8_t old=vm.P&FLAG_C;vm.P=(uint8_t)((vm.P&~FLAG_C)|(v&1u));v=(uint8_t)((v>>1)|(old<<7));break;}
  default:return -1;
 }
 zn(v);if(imm)vm.A=v;else cpu_write(a,v);vm.cycles+=2;break;
 /* Unsupported undocumented opcodes: fail rather than silently corrupt CPU. */
 default:return -1;
 }
 vm.cycles+=1;return 0;
}
static const uint16_t nes_palette[64]={
 0x7bef,0x001f,0x0017,0x3016,0x580e,0x6004,0x5800,0x40c0,0x21a0,0x0260,0x02a0,0x0287,0x020e,0x0000,0x0000,0x0000,
 0xbdf7,0x22df,0x51ff,0x999f,0xb09f,0xc026,0xb820,0x8940,0x6a40,0x3340,0x0400,0x048c,0x04ba,0x4208,0x0000,0x0000,
 0xffff,0x94df,0xc39f,0xeb5f,0xfb9f,0xfb38,0xfad0,0xdbe0,0xbce0,0x85e0,0x5e20,0x5eac,0x66df,0x8410,0x0000,0x0000,
 0xffff,0xd6ff,0xe5ff,0xf5df,0xfddf,0xfdba,0xfdb5,0xf631,0xee2d,0xdf0c,0xcfae,0xc7b4,0xc7ff,0xce79,0x0000,0x0000
};
static uint16_t pixel_background(unsigned x,unsigned y,unsigned *opaque){
 *opaque=0;
 if(!(vm.ppumask&0x08u))return nes_palette[vm.pal[0]&63u];
 unsigned sx=(x+(unsigned)vm.scroll_x)&511u,sy=(y+(unsigned)vm.scroll_y)&479u;
 unsigned page=(sx/256u)+((sy/240u)<<1);
 uint16_t nt_base=(uint16_t)(0x2000u+page*0x400u);
 unsigned tx=(sx%256u)/8u,ty=(sy%240u)/8u;
 unsigned tile=ppu_read((uint16_t)(nt_base+ty*32u+tx));
 unsigned fine_y=sy&7u,fine_x=7u-(sx&7u);
 unsigned pat=((vm.ppuctrl&0x10u)?0x1000u:0u)+tile*16u+fine_y;
 unsigned bit=((ppu_read((uint16_t)pat)>>fine_x)&1u)|(((ppu_read((uint16_t)(pat+8u))>>fine_x)&1u)<<1);
 if(bit==0)return nes_palette[vm.pal[0]&63u];
 unsigned attr=ppu_read((uint16_t)(nt_base+0x3c0u+(ty/4u)*8u+tx/4u));
 unsigned shift=(((ty/2u)&1u)*4u)+(((tx/2u)&1u)*2u);
 unsigned index=((attr>>shift)&3u)*4u+bit;
 *opaque=1;
 return nes_palette[vm.pal[index&31u]&63u];
}
static uint8_t scan_sprites[8],scan_count;
static void prepare_sprites(unsigned y){
 scan_count=0;
 if(!(vm.ppumask&0x10u))return;
 unsigned height=(vm.ppuctrl&0x20u)?16u:8u;
 for(unsigned s=0;s<64u;s++){
  unsigned y0=(unsigned)vm.oam[s*4u]+1u;
  if(y<y0 || y>=y0+height)continue;
  if(scan_count<8u)scan_sprites[scan_count++]=(uint8_t)s;
  else {vm.ppustatus|=0x20u;break;} /* simplified overflow */
 }
}
static uint32_t pixel_sprite(unsigned x,unsigned y,unsigned *behind,unsigned *is_sprite0){
 *behind=0;*is_sprite0=0;
 if(!(vm.ppumask&0x10u))return 0x10000u;
 for(unsigned k=0;k<scan_count;k++){
  unsigned s=scan_sprites[k];
  unsigned y0=(unsigned)vm.oam[s*4u]+1u;
  unsigned x0=(unsigned)vm.oam[s*4u+3u];
  if(x<x0||x>=x0+8u)continue;
  unsigned tile=vm.oam[s*4u+1u],attr=vm.oam[s*4u+2u];
  unsigned xx=x-x0,yy=y-y0;
  if(attr&0x40u)xx=7u-xx;
  if(attr&0x80u)yy=((vm.ppuctrl&0x20u)?15u:7u)-yy;
  unsigned pat;
  if(vm.ppuctrl&0x20u){
   pat=((tile&1u)?0x1000u:0u)+(tile&0xfeu)*16u;
   if(yy>=8u){pat+=16u;yy-=8u;}
   pat+=yy;
  }else pat=((vm.ppuctrl&0x08u)?0x1000u:0u)+tile*16u+yy;
  unsigned b=((ppu_read((uint16_t)pat)>>(7u-xx))&1u)|(((ppu_read((uint16_t)(pat+8u))>>(7u-xx))&1u)<<1);
  if(!b)continue;
  *behind=(attr&0x20u)!=0u;
  *is_sprite0=(s==0u);
  return nes_palette[vm.pal[(16u+((attr&3u)*4u)+b)&31u]&63u];
 }
 return 0x10000u;
}
static void draw(void){
 vm.ppustatus&=(uint8_t)~0x40u;
 for(unsigned y=0;y<H;y++){
 prepare_sprites(y);
 for(unsigned x=0;x<W;x++){
  unsigned opaque=0,behind=0,s0=0;
  uint16_t bg=pixel_background(x,y,&opaque);
  uint32_t sp=pixel_sprite(x,y,&behind,&s0);
  if(s0 && opaque && x<255u)vm.ppustatus|=0x40u;
  vm.frame[y*W+x]=(uint16_t)((sp!=0x10000u && (!behind||!opaque))?sp:bg);
 }
 }
 vm.host->present(vm.host->user,vm.frame,W,H);
}
static int start(const spmp_core_host *host,const char *path){
 if(!host||!host->present||!host->read_entire||!path)return -1;
 zero(&vm,sizeof vm);
 size_t got=0;
 if(host->read_entire(host->user,path,vm.image,sizeof vm.image,&got)||got<16u)return -1;
 if(vm.image[0]!='N'||vm.image[1]!='E'||vm.image[2]!='S'||vm.image[3]!=0x1a)return -1;
 const uint8_t *head=vm.image;
 if((head[7]&0x0cu)==0x08u)return -1; /* NES 2.0 not supported here */
 vm.mapper=(uint8_t)((head[6]>>4u)|(head[7]&0xf0u));
 if(vm.mapper!=0&&vm.mapper!=2&&vm.mapper!=3)return -1;
 vm.prg_bytes=(uint32_t)head[4]*0x4000u;
 vm.chr_bytes=(uint32_t)head[5]*0x2000u;
 unsigned offset=(head[6]&4u)?512u:0u;
 if(vm.prg_bytes<0x4000u||vm.prg_bytes>PRG_CAP||vm.chr_bytes>CHR_CAP)return -1;
 if(16u+offset+vm.prg_bytes+vm.chr_bytes>got)return -1;
 /* Normalize optional trainer out of the memory image. */
 fill(vm.sram,0xff,sizeof vm.sram);
 if(offset){unsigned len=vm.prg_bytes+vm.chr_bytes;
  for(unsigned i=0;i<512u;i++)vm.sram[0x1000u+i]=vm.image[16u+i];
  for(unsigned i=0;i<len;i++)vm.image[16u+i]=vm.image[16u+offset+i];}
 vm.mirror_four=(head[6]&8u)!=0u;vm.mirror_vertical=(head[6]&1u)!=0u;
 vm.battery=(head[6]&2u)!=0u;
 if(vm.mapper==2&&(vm.prg_bytes<0x8000u))return -1;
 if(vm.mapper==3 && (!vm.chr_bytes || vm.chr_bytes<0x4000u))return -1;
 vm.host=host;vm.content=path;vm.A=0;vm.X=0;vm.Y=0;vm.S=0xfd;vm.P=FLAG_I|FLAG_U;
 vm.ppustatus=0;vm.pc=read16(0xfffc);
 if(vm.pc<0x8000u){vm.host=0;return -1;}
 if(vm.battery && host->load_save){size_t loaded=0;
  if(!host->load_save(host->user,"nes",path,vm.sram,sizeof vm.sram,&loaded) && loaded>sizeof vm.sram)return -1;
 }
 vm.live=1;return 0;
}
static int tick(void){
 if(!vm.live)return -1;
 /* Precise PPU scanlines, DMA stalls, APU IRQ and scrolling are not ready.
  * ~29780 NTSC CPU cycles/frame, interpreted using a provisional clock. */
 uint64_t until=vm.cycles+29780u;
 vm.ppustatus|=0x80u;
 if(vm.ppuctrl&0x80u)vm.nmi=1;
 unsigned watchdog=25000u;
 while(vm.cycles<until && watchdog--){if(cpu_step())return -1;}
 if(!watchdog)return -1;
 draw();vm.ppustatus&=(uint8_t)~0x80u;
 return 0;
}
static void stop(void){
 if(vm.live && vm.battery && vm.dirty && vm.host && vm.host->store_save)
  (void)vm.host->store_save(vm.host->user,"nes",vm.content,vm.sram,sizeof vm.sram);
 vm.live=0;vm.host=0;vm.content=0;
}
const spmp_core_ops spmp_nes_ops={start,tick,stop,NULL,NULL,NULL,NULL};
/* Debug hook for synthetic, noncommercial ROM tests only. Not in public HAL. */
uint8_t spmp_nes_test_sram(unsigned a){return vm.sram[a&0x1fffu];}
uint16_t spmp_nes_test_pc(void){return vm.pc;}
