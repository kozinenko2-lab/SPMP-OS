#include "spmp_psx_bios.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int spmp_psx_bios_make_path(char *out,size_t cap,const char *dir,const char *name) {
    size_t nd,nn;int join;
    if (!out || !cap || !dir || !name || !*name) return 0;
    /* Leading slash in name is legacy convention, not an absolute path. */
    while (*name=='/' || *name=='\\') ++name;
    if (!*name) return 0;
    nd=strlen(dir);nn=strlen(name);
    join=nd>0 && dir[nd-1]!='/' && dir[nd-1]!='\\';
    if (nd>=cap || cap-nd<=((size_t)join) || nn>cap-nd-1u-(unsigned)join) return 0;
    memcpy(out,dir,nd);
    if (join) out[nd++]='/';
    memcpy(out+nd,name,nn+1);
    return 1;
}
static uint32_t crc32_bytes(const uint8_t *data,size_t len) {
    uint32_t c=UINT32_C(0xffffffff);
    for(size_t i=0;i<len;i++) {
        c^=data[i];
        for(int j=0;j<8;j++) c=(c>>1)^((c&1u)?UINT32_C(0xedb88320):0);
    }
    return ~c;
}
spmp_psx_bios_status spmp_psx_bios_load(const char *path, uint8_t *out,
                                        size_t cap,spmp_psx_bios_info *info) {
    FILE *file;size_t n;int last;spmp_psx_bios_status status=SPMP_PSX_BIOS_OK;
    const char marker[]="System ROM Version ";
    if(info) {memset(info,0,sizeof(*info));info->region='?';}
    if (!path || !out || cap<SPMP_PSX_BIOS_BYTES) return SPMP_PSX_BIOS_BAD_ARGUMENT;
    memset(out,0,SPMP_PSX_BIOS_BYTES);
    file=fopen(path,"rb");
    if (!file) return SPMP_PSX_BIOS_NOT_FOUND;
    n=fread(out,1,SPMP_PSX_BIOS_BYTES,file);
    if(n!=SPMP_PSX_BIOS_BYTES) {
        status=ferror(file)?SPMP_PSX_BIOS_READ_FAILED:SPMP_PSX_BIOS_WRONG_SIZE;
        goto close_file;
    }
    last=fgetc(file);
    if(last!=EOF) {status=SPMP_PSX_BIOS_WRONG_SIZE;goto close_file;}
    if(ferror(file)) {status=SPMP_PSX_BIOS_READ_FAILED;goto close_file;}
    /* The reset vector at 0xbfc00000 must contain MIPS code, not a blank ROM.
     * Avoid rejecting legitimate region/revision variants using fixed hashes. */
    if((out[0]==0 && out[1]==0 && out[2]==0 && out[3]==0) ||
       (out[0]==255 && out[1]==255 && out[2]==255 && out[3]==255)) {
        status=SPMP_PSX_BIOS_INVALID;goto close_file;
    }
    if (info) {
        info->crc32=crc32_bytes(out,SPMP_PSX_BIOS_BYTES);
        info->known_scph101_v45=info->crc32==SPMP_PSX_SCPH101_V45_CRC32;
        for(size_t at=SPMP_PSX_BIOS_BYTES-4096;at+sizeof(marker)<SPMP_PSX_BIOS_BYTES;at++) {
            if (memcmp(out+at,marker,sizeof(marker)-1)==0) {
                size_t used=0;at+=sizeof(marker)-1;
                while (at<SPMP_PSX_BIOS_BYTES && used<sizeof(info->version)-1) {
                    uint8_t b=out[at++];
                    if(b<32 || b>126) break;
                    info->version[used++]=(char)b;
                }
                info->version[used]='\0';
                if(used && info->version[used-1]=='A') info->region='A';
                if(used && info->version[used-1]=='E') info->region='E';
                if(used && info->version[used-1]=='J') info->region='J';
                break;
            }
        }
    }
close_file:
    if(fclose(file)!=0 && status==SPMP_PSX_BIOS_OK) status=SPMP_PSX_BIOS_READ_FAILED;
    if(status!=SPMP_PSX_BIOS_OK) memset(out,0,SPMP_PSX_BIOS_BYTES);
    return status;
}
const char *spmp_psx_bios_status_text(spmp_psx_bios_status status) {
    switch(status) {
    case SPMP_PSX_BIOS_OK:return "BIOS ready";
    case SPMP_PSX_BIOS_BAD_ARGUMENT:return "invalid BIOS destination";
    case SPMP_PSX_BIOS_PATH_TOO_LONG:return "BIOS path too long";
    case SPMP_PSX_BIOS_NOT_FOUND:return "BIOS file not found";
    case SPMP_PSX_BIOS_WRONG_SIZE:return "BIOS must be exactly 524288 bytes";
    case SPMP_PSX_BIOS_READ_FAILED:return "BIOS file read error";
    case SPMP_PSX_BIOS_INVALID:return "BIOS reset vector invalid";
    default:return "BIOS error";
    }
}
