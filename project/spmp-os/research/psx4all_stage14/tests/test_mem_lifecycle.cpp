#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include "../vendor/psx4all-dingoo-master/src/common.h"
#include "../vendor/psx4all-dingoo-master/src/mem.h"

PsxConfig Config = {};
void SysPrintf(const char *fmt, ...) { (void)fmt; }
u8 psxHwRead8(u32) { return 0; }
u16 psxHwRead16(u32) { return 0; }
u32 psxHwRead32(u32) { return 0; }
void psxHwWrite8(u32,u8) {}
void psxHwWrite16(u32,u16) {}
void psxHwWrite32(u32,u32) {}

static void require(int cond, const char *desc, unsigned iter) {
    if (!cond) { std::fprintf(stderr,"MEM FAIL #%u: %s\n",iter,desc); std::exit(1); }
}
static uint32_t rng=0x1327a1dfu;
static uint32_t random32() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }
int main() {
    static_assert(sizeof(u32)==4, "PS1 u32 must be 32 bit");
    static_assert(sizeof(s32)==4, "PS1 s32 must be 32 bit");
    Config.HLE=1;
    psxMemReset(); // calling before Init must not dereference null buffers
    psxMemShutdown();
    unsigned checks=0;
    for (unsigned session=0;session<100;session++) {
        require(psxMemInit()==0,"init",session);
        require(psxMemInit()!=0,"double init rejected",session);
        require(psxMemRLUT && psxMemWLUT,"LUT allocated",session);
        psxMemReset();
        require(psxMemRead32(0x00000000)==0,"RAM reset",session);++checks;
        // 2 MB RAM mirrors at 0x80000000, 0xa0000000 and every 2MB within 8MB.
        for (unsigned k=0;k<1200;k++) {
            uint32_t address=random32() & 0x001ffffcu;
            uint32_t word=random32();
            psxMemWrite32(address,word);
            require(psxMemRead32(address)==word,"native write/read",k); ++checks;
            require(psxMemRead32(0x80000000u|address)==word,"KSEG0 mirror",k); ++checks;
            require(psxMemRead32(0xa0000000u|address)==word,"KSEG1 mirror",k); ++checks;
            require(psxMemRead32(0x00600000u|address)==word,"4x RAM mirror",k); ++checks;
        }
        psxMemWrite8(0x1f000100,0x8eu);
        require(psxMemRead8(0x1f000100)==0x8eu,"parallel port",session); ++checks;
        psxMemWrite16(0x1f800010,0xb05eu);
        require(psxMemRead16(0x1f800010)==0xb05eu,"scratchpad",session); ++checks;
        psxMemWrite32(0x00001000,0x12345678u);
        psxMemWrite32(0xfffe0130,0x800u); // isolate cache => write LUT cleared
        require(psxMemWLUT[0x0000]==0,"cache isolate",session); ++checks;
        psxMemWrite32(0xfffe0130,0x1e988u);
        require(psxMemWLUT[0x0000]!=0,"cache isolate restored",session); ++checks;
        psxMemShutdown();
        require(!psxMemRLUT && !psxMemWLUT && !psxM && !psxP && !psxR && !psxH,"shutdown null",session);++checks;
        psxMemShutdown(); // no double free
    }
    std::printf("PSX4ALL Stage14 memory lifecycle: %u assertions; 100 sessions; 120000 randomized 32-bit guest writes PASS\n",checks);
}
