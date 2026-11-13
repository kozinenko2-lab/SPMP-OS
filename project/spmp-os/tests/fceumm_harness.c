/* Standalone end-to-end SPMP-OS/FCEUmm integration test.
 * Synthetic ROMs are generated in tools/make_nes_test_roms.py.
 * This is NOT a commercial ROM compatibility claim.
 */
#include "spmp_cores.h"
#include "libretro.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern const spmp_core_ops spmp_fceumm_ops;

typedef struct {
    unsigned frames, audio_calls, loads, stores, rate;
    size_t samples;
    uint16_t first_pixel;
    uint8_t save[8192];
    int have_save;
} test_state;
static void present(void *p, const uint16_t *pixels,unsigned w,unsigned h) {
    test_state *s=(test_state*)p;
    assert(w==256 && h>0 && h<=240 && pixels);
    s->frames++;
    s->first_pixel=pixels[0];
}
static void sound(void *p,const int16_t *samples,size_t frames,unsigned rate) {
    test_state *s=(test_state*)p;
    assert(samples && frames && rate>=8000 && rate<=96000);
    s->samples+=frames;
    s->rate=rate;
    s->audio_calls++;
}
static uint32_t buttons(void *p){(void)p;return 0;}
static int read_file(void *p,const char *path,uint8_t *out,size_t cap,size_t *size) {
    (void)p;
    FILE *f=fopen(path,"rb");if(!f)return -1;
    size_t read=fread(out,1,cap,f);
    int over=fgetc(f),error=ferror(f);
    fclose(f);
    if(over!=EOF||error)return -1;
    *size=read;
    return 0;
}
static int load_save(void *p,const char *core,const char *game,uint8_t *out,size_t cap,size_t *size) {
    test_state *s=(test_state*)p;
    assert(strcmp(core,"nes")==0 && game);
    s->loads++;
    if(!s->have_save||cap>sizeof s->save)return -1;
    memcpy(out,s->save,cap);
    *size=cap;
    return 0;
}
static int store_save(void *p,const char *core,const char *game,const uint8_t *in,size_t n) {
    test_state *s=(test_state*)p;
    assert(strcmp(core,"nes")==0 && game);
    assert(n<=sizeof s->save);
    memcpy(s->save,in,n);
    s->stores++;
    s->have_save=1;
    return 0;
}
static void run(test_state *state,const char *rom,unsigned frames) {
    spmp_core_host callbacks={.user=state,.present=present,.buttons=buttons,.read_entire=read_file,.audio_pcm=sound,.load_save=load_save,.store_save=store_save};
    unsigned before=state->frames;
    assert(spmp_fceumm_ops.start(&callbacks,rom)==0);
    for(unsigned i=0;i<frames;i++)assert(spmp_fceumm_ops.tick()==0);
    assert(state->frames==before+frames);
    /* FCEUmm's actual libretro savestate implementation */
    size_t count=spmp_fceumm_ops.state_size();
    assert(count>0&&count<2u*1024u*1024u);
    uint8_t *data=(uint8_t*)malloc(count);assert(data);
    assert(!spmp_fceumm_ops.serialize(data,count));
    assert(!spmp_fceumm_ops.unserialize(data,count));
    free(data);
    assert(state->audio_calls>0 && state->samples>0);
    spmp_fceumm_ops.stop();
}
int main(void) {
    test_state normal={0};
    run(&normal,"samples/nes_cpu_ppu_smoke.nes",3);
    assert(normal.have_save && normal.save[0]==0xa5 && normal.loads==1 && normal.stores==1);
    assert(normal.rate==48000);
    /* Stop / start must restore game SRAM and not leak stale callbacks. */
    run(&normal,"samples/nes_cpu_ppu_smoke.nes",3);
    assert(normal.loads==2 && normal.stores==2 && normal.save[0]==0xa5);
    test_state uxrom={0};
    run(&uxrom,"samples/nes_uxrom_smoke.nes",3);
    assert(uxrom.save[1]==0x42);
    test_state cnrom={0};
    run(&cnrom,"samples/nes_cnrom_smoke.nes",3);
    assert(cnrom.frames==3);
    spmp_core_host callbacks={.user=&normal,.present=present,.buttons=buttons,.read_entire=read_file,.audio_pcm=sound,.load_save=load_save,.store_save=store_save};
    assert(spmp_fceumm_ops.start(&callbacks,"samples/not-found.nes")<0);
    assert(spmp_fceumm_ops.tick()<0);
    /* Failed start must not block a subsequent clean start. */
    run(&normal,"samples/nes_cpu_ppu_smoke.nes",1);
    printf("FCEUmm: 3 mappers, CPU/SRAM, RGB565, stereo PCM at %u Hz, repeat load, failure recovery OK\n",normal.rate);
    return 0;
}
