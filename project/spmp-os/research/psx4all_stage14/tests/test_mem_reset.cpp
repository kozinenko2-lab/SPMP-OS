#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <string>
#include "../vendor/psx4all-dingoo-master/src/common.h"
#include "../vendor/psx4all-dingoo-master/src/mem.h"

PsxConfig Config = {};
void SysPrintf(const char *fmt, ...) {
    va_list args; va_start(args,fmt); vfprintf(stderr,fmt,args);va_end(args);
}
#define REQ(x) do { if (!(x)) {std::fprintf(stderr,"MEM_RESET FAIL line %d: %s\n",__LINE__,#x);return 1;} } while (0)
int main(int argc,char **argv) {
    REQ(argc==2);
    std::string file(argv[1]);
    std::size_t pos=file.find_last_of('/');
    std::string dir=pos==std::string::npos?"./":file.substr(0,pos+1);
    std::string name=pos==std::string::npos?file:file.substr(pos+1);
    REQ(dir.size()<sizeof(Config.BiosDir));
    REQ(name.size()<sizeof(Config.Bios));
    std::strcpy(Config.BiosDir,dir.c_str());
    std::strcpy(Config.Bios,name.c_str());
    psxM=(s8*)std::malloc(0x200000); psxP=(s8*)std::malloc(0x10000);
    psxR=(s8*)std::malloc(0x80000);
    REQ(psxM && psxP && psxR);
    std::memset(psxM,0xab,0x200000);
    std::memset(psxP,0xab,0x10000);
    std::memset(psxR,0xab,0x80000);
    Config.HLE=0;
    psxMemReset();
    REQ(Config.HLE==0);
    REQ((unsigned char)psxR[0]==0x13);
    REQ((unsigned char)psxR[3]==0x3c);
    REQ((unsigned char)psxM[0]==0);
    REQ((unsigned char)psxP[0]==0);
    /* Re-entry without BIOS must clean ROM bytes and select safe fallback. */
    std::strcpy(Config.Bios,"missing-firmware-not-real.bin");
    psxMemReset();
    REQ(Config.HLE==1);
    REQ((unsigned char)psxR[0]==0);
    REQ((unsigned char)psxR[0x7ffff]==0);
    std::free(psxM);std::free(psxP);std::free(psxR);
    psxM=psxP=psxR=nullptr;
    std::puts("PSX4ALL psxMemReset: real BIOS loaded and missing BIOS fallback verified");
    return 0;
}
