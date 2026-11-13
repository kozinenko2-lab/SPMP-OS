/* NES baseline deterministic tests (not a commercial game compatibility test). */
#include "spmp_cores.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const spmp_core_ops spmp_nes_ops;
extern uint8_t spmp_nes_test_sram(unsigned);
extern uint16_t spmp_nes_test_pc(void);

typedef struct {
 unsigned frames,saves,loads;
 uint16_t px0,px1;
 uint8_t save[8192];
} context;
static void frame(void *u,const uint16_t *pixels,unsigned w,unsigned h){
 context *c=u;
 assert(w==256&&h==240);
 c->frames++;c->px0=pixels[0];c->px1=pixels[1];
}
static uint32_t buttons(void *u){(void)u;return 0;}
static int readfile(void *u,const char *p,uint8_t *to,size_t cap,size_t *size){
 (void)u;
 FILE *fp=fopen(p,"rb");if(!fp)return -1;
 size_t got=fread(to,1,cap,fp);
 int extra=fgetc(fp);
 int err=ferror(fp);
 fclose(fp);
 if(extra!=EOF||err)return -1;
 *size=got;return 0;
}
static int load(void *u,const char *core,const char *path,uint8_t *dst,size_t cap,size_t *size){
 context *c=u;assert(!strcmp(core,"nes")&&path);assert(cap==sizeof c->save);
 c->loads++;
 if(c->saves){memcpy(dst,c->save,cap);*size=cap;return 0;}
 return -1;
}
static int store(void *u,const char *core,const char *path,const uint8_t *src,size_t size){
 context *c=u;assert(!strcmp(core,"nes")&&path);assert(size==sizeof c->save);
 memcpy(c->save,src,size);c->saves++;return 0;
}
static void go(context *c,const char *file,unsigned count,unsigned saveexpected){
 spmp_core_host host={.user=c,.present=frame,.buttons=buttons,.read_entire=readfile,.load_save=load,.store_save=store};
 assert(!spmp_nes_ops.start(&host,file));
 for(unsigned i=0;i<count;i++)assert(spmp_nes_ops.tick()==0);
 assert(c->frames==count);
 spmp_nes_ops.stop();
 assert(c->saves==saveexpected);
}
int main(void){
 context c={0};
 go(&c,"samples/nes_cpu_ppu_smoke.nes",2,1);
 assert(c.px0 != c.px1);
 assert(c.save[0]==0xa5);
 assert(c.loads==1);
 c.frames=0;
 go(&c,"samples/nes_cpu_ppu_smoke.nes",2,1); /* no write if unchanged */
 assert(c.loads==2);
 context d={0};
 go(&d,"samples/nes_uxrom_smoke.nes",2,1);
 assert(d.save[1]==0x42); /* switched low PRG bank then read marker */
 context e={0};
 go(&e,"samples/nes_cnrom_smoke.nes",2,0);
 assert(e.px0!=e.px1); /* CHR bank 1 tile data was visible */
 /* An unsupported mapper or a malformed ROM must fail before execution. */
 puts("NES core: CPU/PPU mappers 0/2/3 + SRAM tests passed");
 return 0;
}
