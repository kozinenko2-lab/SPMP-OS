#include "spmp_media.h"
#include <string.h>
spmp_format spmp_probe_header(const uint8_t *b,size_t n){
 if(!b)return SPMP_FMT_UNKNOWN;
 if(n>=8 && !memcmp(b,"NGame1.0",8))return SPMP_FMT_NGAME;
 if(n>=4 && !memcmp(b,"\x7f""ELF",4))return SPMP_FMT_ARM_ELF; /* architecture must be inspected separately */
 if(n>=8 && !memcmp(b,"\x89""PNG\r\n\x1a\n",8))return SPMP_FMT_PNG;
 if(n>=3 && b[0]==0xff && b[1]==0xd8 && b[2]==0xff)return SPMP_FMT_JPEG;
 if(n>=6 && (!memcmp(b,"GIF87a",6)||!memcmp(b,"GIF89a",6)))return SPMP_FMT_GIF;
 if(n>=12 && !memcmp(b,"RIFF",4) && !memcmp(b+8,"AVI ",4))return SPMP_FMT_AVI;
 if(n>=12 && !memcmp(b,"RIFF",4) && !memcmp(b+8,"WAVE",4))return SPMP_FMT_WAV;
 if(n>=12 && !memcmp(b+4,"ftyp",4))return SPMP_FMT_MP4;
 if(n>=3 && !memcmp(b,"ID3",3))return SPMP_FMT_MP3;
 if(n>=2 && b[0]==0xff && (b[1]&0xe0)==0xe0 && (b[1]&0x18)!=0x08)return SPMP_FMT_MP3;
 return SPMP_FMT_UNKNOWN;
}
const char *spmp_format_name(spmp_format x){
 static const char *names[]={"unknown","MP3","MP4-container","AVI-container","JPEG","PNG","GIF","WAV","NGame1.0","ELF"};
 return x<=SPMP_FMT_ARM_ELF?names[x]:"unknown";
}
