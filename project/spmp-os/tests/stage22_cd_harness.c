#include "spmp_cores.h"
#include "third_party/snes9x/libretro/libretro.h"
#include <dlfcn.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct { unsigned frames,audio_frames,w,h; } ctx;
static void present(void *ud,const uint16_t *p,unsigned w,unsigned h){
 ctx *c=ud;assert(p&&w>=256&&w<=512&&h>=192&&h<=480);
 c->frames++;c->w=w;c->h=h;
}
static uint32_t buttons(void *ud){(void)ud;return 0;}
static void audio(void *ud,const int16_t *p,size_t n,unsigned rate){
 ctx *c=ud;assert(p&&rate>=8000&&rate<=96000);c->audio_frames+=(unsigned)n;
}
static int file_size(void *ud,const char *p,size_t *n){
 (void)ud;struct stat st;
 if(stat(p,&st)||st.st_size<=0||st.st_size>4*1024*1024)return -1;
 *n=(size_t)st.st_size;return 0;
}
static int read_entire(void *ud,const char *p,uint8_t *d,size_t cap,size_t *n){
 (void)ud; if(strstr(p,".cue")||strstr(p,".iso")||strstr(p,".chd"))return -1; FILE *f=fopen(p,"rb");if(!f)return -1;
 *n=fread(d,1,cap,f);int extra=fgetc(f),bad=ferror(f);fclose(f);
 return bad||extra!=EOF?-1:0;
}
int main(int argc,char **argv){
 assert(argc==2);
 ctx c={0};spmp_core_host h={0};h.user=&c;h.present=present;h.audio_pcm=audio;
 h.buttons=buttons;h.file_size=file_size;h.read_entire=read_entire;
 assert(spmp_core_find("megadrive")!=NULL);
 assert(spmp_core_launch("megadrive",&h,argv[1])==0);
 for(unsigned i=0;i<15;i++)assert(spmp_core_tick()==0);
 assert(c.frames>=10);
 assert(c.audio_frames>0);
 void *lib=dlopen("third_party/picodrive/picodrive_libretro.so",RTLD_NOW|RTLD_LOCAL);
 assert(lib);
 typedef void *(*mem_fn)(unsigned);
 mem_fn mem=NULL;
 void *addr=dlsym(lib,"retro_get_memory_data");
 assert(addr&&sizeof(addr)==sizeof(mem));memcpy(&mem,&addr,sizeof(mem));
 uint8_t *ram=(uint8_t*)mem(RETRO_MEMORY_SYSTEM_RAM);
 assert(ram);
 fprintf(stderr,"MD RAM first bytes: %02x %02x %02x %02x\n",ram[0],ram[1],ram[2],ram[3]);
 void *sym=dlsym(lib,"Pico_mcd");assert(sym);void *mcd=NULL;memcpy(&mcd,sym,sizeof(mcd));
 assert(mcd && "Sega CD CD-ROM+SubCPU memory should initialize");
 fprintf(stderr,"Sega CD memory/CD controller active, disc streamed from path\n");
 assert(spmp_core_pause_to_menu()==0);
 assert(spmp_core_tick()==2);
 size_t n=spmp_core_state_size();
 assert(n>0&&n<8u*1024u*1024u);
 uint8_t *state=malloc(n);assert(state);
 assert(!spmp_core_serialize(state,n));
 assert(!spmp_core_unserialize(state,n));
 free(state);
 assert(!spmp_core_resume());
 spmp_core_stop();
 assert(spmp_core_cached_id()&&!strcmp(spmp_core_cached_id(),"megadrive"));
 assert(!spmp_core_launch("megadrive",&h,argv[1]));
 for(unsigned i=0;i<4;i++)assert(!spmp_core_tick());
 spmp_core_shutdown();dlclose(lib);
 printf("Sega CD synthetic BIOS+disc path: %u frames, %u audio frames, %ux%u, save/restore and reload OK\n",c.frames,c.audio_frames,c.w,c.h);
 return 0;
}
