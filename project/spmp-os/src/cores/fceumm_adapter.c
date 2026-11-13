/* SPMP-OS NES adapter for FCEUmm libretro (GPL-2.0, original sources in
 * third_party/fceumm).  No RetroArch or NGame ABI is required by this API.
 * Host proof-of-integration only; RAM limits and device HAL unverified.
 */
#include "spmp_cores.h"
#include "libretro.h"
#include <stdlib.h>
#include <string.h>

/* Temporary cap until the future VFS exposes a file length/stream interface. */
#define SPMP_FCEUMM_MAX_ROM (4u * 1024u * 1024u)
#define SPMP_FCEUMM_MAX_SAVE (256u * 1024u)
#define SPMP_FCEUMM_MAX_W 256u
#define SPMP_FCEUMM_MAX_H 240u
static const spmp_core_host *host_api;
static const char *game_id;
static uint8_t *rom_data;
static uint16_t copy_frame[SPMP_FCEUMM_MAX_W*SPMP_FCEUMM_MAX_H];
static int active;
static unsigned pcm_rate=48000;
static int pixel_format_ok;
static unsigned frame_count;

static bool environment(unsigned cmd, void *data) {
    switch(cmd) {
    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: {
        enum retro_pixel_format fmt = *(const enum retro_pixel_format *)data;
        pixel_format_ok = fmt == RETRO_PIXEL_FORMAT_RGB565;
        return pixel_format_ok != 0;
    }
    case RETRO_ENVIRONMENT_GET_CAN_DUPE: if(data) *(bool*)data=true; return true;
    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE: if(data) *(bool*)data=false; return true;
    case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
    case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS:
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL:
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2:
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL:
    case RETRO_ENVIRONMENT_SET_VARIABLES:
    case RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS:
    case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
    case RETRO_ENVIRONMENT_SET_CONTENT_INFO_OVERRIDE:
        return true;
    default: return false;
    }
}
static void video(const void *data, unsigned w, unsigned h, size_t pitch) {
    if (!host_api || !data || !pixel_format_ok || w == 0 || h == 0 ||
        w > SPMP_FCEUMM_MAX_W || h > SPMP_FCEUMM_MAX_H || pitch < w*2u) return;
    if (pitch == w*2u) host_api->present(host_api->user, (const uint16_t*)data,w,h);
    else {
        for (unsigned y=0;y<h;y++)
            memcpy(copy_frame+y*w,(const uint8_t*)data+y*pitch,w*2u);
        host_api->present(host_api->user,copy_frame,w,h);
    }
    frame_count++;
}
static size_t audio_batch(const int16_t *samples, size_t frames) {
    if(host_api && host_api->audio_pcm && samples && frames)
        host_api->audio_pcm(host_api->user,samples,frames,pcm_rate);
    return frames;
}
static void audio_one(int16_t l,int16_t r) { int16_t sample[2]={l,r}; audio_batch(sample,1); }
static void input_poll(void) { }
static int16_t input(unsigned port,unsigned device,unsigned index,unsigned id) {
    (void)index;
    if(port || device!=RETRO_DEVICE_JOYPAD || !host_api || !host_api->buttons)return 0;
    uint32_t btn=host_api->buttons(host_api->user);
    unsigned bit;
    switch(id) {
    case RETRO_DEVICE_ID_JOYPAD_UP: bit=0;break;
    case RETRO_DEVICE_ID_JOYPAD_DOWN:bit=1;break;
    case RETRO_DEVICE_ID_JOYPAD_LEFT:bit=2;break;
    case RETRO_DEVICE_ID_JOYPAD_RIGHT:bit=3;break;
    case RETRO_DEVICE_ID_JOYPAD_B:bit=4;break;
    case RETRO_DEVICE_ID_JOYPAD_A:bit=5;break;
    case RETRO_DEVICE_ID_JOYPAD_START:bit=6;break;
    case RETRO_DEVICE_ID_JOYPAD_SELECT:bit=7;break;
    default:return 0;
    }
    return (btn&(1u<<bit))?1:0;
}
static int start(const spmp_core_host *host,const char *path) {
    size_t got=0;
    struct retro_game_info info;
    struct retro_system_av_info av;
    if(active || !host || !host->present || !host->read_entire || !path) return -1;
    rom_data=(uint8_t*)malloc(SPMP_FCEUMM_MAX_ROM);
    if(!rom_data)return -1;
    if(host->read_entire(host->user,path,rom_data,SPMP_FCEUMM_MAX_ROM,&got) || got<16)goto error;
    host_api=host;
    game_id=path;
    pixel_format_ok=0;
    frame_count=0;
    retro_set_environment(environment);
    retro_set_video_refresh(video);
    retro_set_audio_sample(audio_one);
    retro_set_audio_sample_batch(audio_batch);
    retro_set_input_poll(input_poll);
    retro_set_input_state(input);
    retro_init();
    memset(&info,0,sizeof info);
    info.path=path;
    info.data=rom_data;
    info.size=got;
    if(!retro_load_game(&info)) {retro_deinit(); goto error;}
    if(!pixel_format_ok) {retro_unload_game(); retro_deinit();goto error;}
    retro_get_system_av_info(&av);
    if(av.timing.sample_rate>=8000 && av.timing.sample_rate<=96000)
        pcm_rate=(unsigned)(av.timing.sample_rate+0.5);
    size_t save_len=retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
    uint8_t *save=(uint8_t*)retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    if(save && save_len && save_len<=SPMP_FCEUMM_MAX_SAVE && host_api->load_save) {
        size_t n=0;
        host_api->load_save(host_api->user,"nes",path,save,save_len,&n);
    }
    active=1;
    return 0;
error:
    free(rom_data);rom_data=0;host_api=0;game_id=0;return -1;
}
static int tick(void) {
    if(!active || !host_api)return -1;
    retro_run();
    return 0;
}
static void stop(void) {
    if(!active)return;
    size_t len=retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
    const uint8_t *save=(const uint8_t*)retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    if(save && len && len<=SPMP_FCEUMM_MAX_SAVE && host_api && host_api->store_save)
        host_api->store_save(host_api->user,"nes",game_id,save,len);
    retro_unload_game();
    retro_deinit();
    free(rom_data);rom_data=0;
    host_api=0;game_id=0;active=0;
}
static size_t fce_state_size(void){return active?retro_serialize_size():0;}
static int fce_serialize(void *dst,size_t n){return active&&retro_serialize(dst,n)?0:-1;}
static int fce_unserialize(const void *src,size_t n){return active&&retro_unserialize(src,n)?0:-1;}
const spmp_core_ops spmp_fceumm_ops={start,tick,stop,fce_state_size,fce_serialize,fce_unserialize,NULL};
