/* No Sony BIOS or game data required: synthetic 512 KiB fixture. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "../vendor/psx4all-dingoo-master/src/spmp_port/spmp_psx_bios.h"
static int checks;
#define REQ(x) do { if (!(x)) { fprintf(stderr,"FAIL BIOS:%d: %s\n",__LINE__,#x);exit(1); } ++checks; } while (0)
static uint8_t rom[SPMP_PSX_BIOS_BYTES];
static void fixture(const char *p,size_t n,int mode) {
    FILE *f=fopen(p,"wb"); REQ(f!=NULL);
    uint8_t buf[4096];
    static const char marker[]="System ROM Version 9.9 01/01/99 E";
    for (size_t start=0;start<n;start+=sizeof(buf)) {
        size_t amount=n-start<sizeof(buf)?n-start:sizeof(buf);
        memset(buf,0,amount);
        if (mode) {
            for(size_t i=0;i<amount;i++) {
                size_t at=start+i;
                if(at==0) buf[i]=0x13;
                if(at==2) buf[i]=0x08;
                if(at==3) buf[i]=0x3c;
                if(at>=SPMP_PSX_BIOS_BYTES-128 &&
                   at<SPMP_PSX_BIOS_BYTES-128+sizeof(marker))
                    buf[i]=(uint8_t)marker[at-(SPMP_PSX_BIOS_BYTES-128)];
            }
        }
        REQ(fwrite(buf,1,amount,f)==amount);
    }
    REQ(fclose(f)==0);
}
int main(void) {
    char path[128],large[128],small[128],bad[128];
    spmp_psx_bios_info m;
    REQ(spmp_psx_bios_make_path(path,sizeof(path),"/tmp/","scph.bin"));
    REQ(!strcmp(path,"/tmp/scph.bin"));
    REQ(spmp_psx_bios_make_path(path,sizeof(path),"/tmp","/scph.bin"));
    REQ(!strcmp(path,"/tmp/scph.bin"));
    REQ(!spmp_psx_bios_make_path(path,5,"/very/long","file"));
    snprintf(path,sizeof(path),"/tmp/spmp_bios_ok_%ld",(long)getpid());
    snprintf(small,sizeof(small),"/tmp/spmp_bios_small_%ld",(long)getpid());
    snprintf(large,sizeof(large),"/tmp/spmp_bios_large_%ld",(long)getpid());
    snprintf(bad,sizeof(bad),"/tmp/spmp_bios_bad_%ld",(long)getpid());
    fixture(path,SPMP_PSX_BIOS_BYTES,1);
    REQ(spmp_psx_bios_load(path,rom,sizeof(rom),&m)==SPMP_PSX_BIOS_OK);
    REQ(m.region=='E' && strstr(m.version,"9.9 01/01/99 E")!=NULL);
    REQ(rom[0]==0x13 && rom[3]==0x3c);
    REQ(!m.known_scph101_v45);
    fixture(small,100,1); fixture(large,SPMP_PSX_BIOS_BYTES+1,1);
    fixture(bad,SPMP_PSX_BIOS_BYTES,0);
    REQ(spmp_psx_bios_load(small,rom,sizeof(rom),&m)==SPMP_PSX_BIOS_WRONG_SIZE);
    REQ(rom[0]==0 && rom[SPMP_PSX_BIOS_BYTES-1]==0);
    REQ(spmp_psx_bios_load(large,rom,sizeof(rom),&m)==SPMP_PSX_BIOS_WRONG_SIZE);
    REQ(spmp_psx_bios_load(bad,rom,sizeof(rom),&m)==SPMP_PSX_BIOS_INVALID);
    REQ(spmp_psx_bios_load("/no-such-file-spmp-os",rom,sizeof(rom),&m)==SPMP_PSX_BIOS_NOT_FOUND);
    REQ(spmp_psx_bios_load(path,rom,100,&m)==SPMP_PSX_BIOS_BAD_ARGUMENT);
    REQ(unlink(path)==0);REQ(unlink(small)==0);REQ(unlink(large)==0);REQ(unlink(bad)==0);
    printf("SPMP PSX BIOS parser: %d assertions, synthetic fixture only\n",checks);
    return 0;
}
