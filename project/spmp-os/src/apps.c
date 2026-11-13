#include "spmp_apps.h"
static const spmp_app apps[]={
 {"games","GAMES","ROM browser, native ports and emulators",SPMP_HOST_PROTOTYPE},
 {"music","MUSIC","Playlists; MP3/AAC/WMA/FLAC after codec integration",SPMP_PLANNED},
 {"video","VIDEO","MP4/AVI container parsing and codec drivers",SPMP_PLANNED},
 {"photos","PHOTOS","JPEG/PNG/GIF decoding and slideshow",SPMP_PLANNED},
 {"radio","FM RADIO","FM tuner, wired headphones as antenna",SPMP_PLANNED},
 {"files","FILES","Read-only host directory browser",SPMP_HOST_PROTOTYPE},
 {"themes","THEMES","RGB565 palette themes from INI",SPMP_HOST_PROTOTYPE},
 {"settings","SETTINGS","Button mapping import and OS preferences",SPMP_HOST_PROTOTYPE},
 {"reader","READER","Text/ebook viewer",SPMP_PLANNED}
};
const spmp_app *spmp_get_apps(size_t *n) {if(n)*n=sizeof apps/sizeof apps[0];return apps;}
