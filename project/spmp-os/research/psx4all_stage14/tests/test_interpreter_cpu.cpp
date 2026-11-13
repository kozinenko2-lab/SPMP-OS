#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include "../vendor/psx4all-dingoo-master/src/common.h"
#include "../vendor/psx4all-dingoo-master/src/mem.h"
void psxBNE();
PsxConfig Config = {};
int PsxCycleMult=2;
void SysPrintf(const char *fmt, ...) { (void)fmt; }
u8 psxHwRead8(u32) { return 0; }
u16 psxHwRead16(u32) { return 0; }
u32 psxHwRead32(u32) { return 0; }
void psxHwWrite8(u32,u8) {}
void psxHwWrite16(u32,u16) {}
void psxHwWrite32(u32,u32) {}
void update_hw(u32) {}
u32 psxException(u32, u32) { std::fprintf(stderr,"Unexpected PSX exception\n");std::abort(); }
// The test exercises GPR integer CPU only. GTE calls abort rather than mock success.
void gteMFC2(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteMFC2\n"); std::abort(); }
void gteCFC2(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteCFC2\n"); std::abort(); }
void gteMTC2(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteMTC2\n"); std::abort(); }
void gteCTC2(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteCTC2\n"); std::abort(); }
void gteLWC2(u32 code, u32 pc){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteLWC2\n"); std::abort(); }
void gteSWC2(u32 code, u32 pc){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteSWC2\n"); std::abort(); }
void gteRTPS(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteRTPS\n"); std::abort(); }
void gteOP(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteOP\n"); std::abort(); }
void gteNCLIP(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteNCLIP\n"); std::abort(); }
void gteDPCS(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteDPCS\n"); std::abort(); }
void gteINTPL(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteINTPL\n"); std::abort(); }
void gteMVMVA(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteMVMVA\n"); std::abort(); }
void gteNCDS(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteNCDS\n"); std::abort(); }
void gteNCDT(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteNCDT\n"); std::abort(); }
void gteCDP(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteCDP\n"); std::abort(); }
void gteNCCS(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteNCCS\n"); std::abort(); }
void gteCC(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteCC\n"); std::abort(); }
void gteNCS(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteNCS\n"); std::abort(); }
void gteNCT(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteNCT\n"); std::abort(); }
void gteSQR(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteSQR\n"); std::abort(); }
void gteDCPL(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteDCPL\n"); std::abort(); }
void gteDPCT(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteDPCT\n"); std::abort(); }
void gteAVSZ3(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteAVSZ3\n"); std::abort(); }
void gteAVSZ4(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteAVSZ4\n"); std::abort(); }
void gteRTPT(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteRTPT\n"); std::abort(); }
void gteGPF(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteGPF\n"); std::abort(); }
void gteGPL(u32 code){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteGPL\n"); std::abort(); }
void gteNCCT(){ std::fprintf(stderr,"unimplemented GTE stub invoked: gteNCCT\n"); std::abort(); }
static void require(bool ok,const char *what) { if(!ok){std::fprintf(stderr,"CPU SMOKE FAIL: %s\n",what); std::exit(1);} }
static u32 I(unsigned op, unsigned rs, unsigned rt, unsigned imm) { return (op<<26)|(rs<<21)|(rt<<16)|(imm&0xffff); }
static u32 R(unsigned rs,unsigned rt,unsigned rd,unsigned sa,unsigned fn){return (rs<<21)|(rt<<16)|(rd<<11)|(sa<<6)|fn;}
int main(){
    static_assert(sizeof(psxGPRRegs)==34*4, "MIPS register layout 32-bit");
    static_assert(sizeof(psxCP0Regs)==32*4, "COP0 register layout 32-bit");
    Config.HLE=1;
    require(psxMemInit()==0,"psxMemInit");
    psxMemReset();
    require(psxInt.Init()==0,"intInit");
    require(psxRegs!=nullptr,"psxRegs allocated");
    unsigned cases=4000;
    if (std::getenv("PSX_CPU_STRESS")) cases=200000;
    uint32_t seed=0x735a1fd1u;
    for(unsigned test=0;test<cases;++test){
        seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
        u32 value=(test%997u==0u)?0xffffffffu:seed;
        u32 expected=value+1u;
        std::memset(psxRegs,0,sizeof(*psxRegs));
        // Scratch test program; branch exits one executeBlock; delay slot must execute.
        // 0: lui t0, 0x1234; 4: ori t0,t0,0x5678; 8: sw t0,0x100(zero)
        // c: lw t1,0x100(zero); 10: addiu t2,t1,1; 14: bne t2,zero,+1
        // 18: addiu t3,zero,7 (delay); 1c: addiu t4,zero,3 (target)
        psxMemWrite32(0x0, I(15,0,8,value>>16));
        psxMemWrite32(0x4, I(13,8,8,value&0xffff));
        psxMemWrite32(0x8, I(43,0,8,0x100));
        psxMemWrite32(0xc, I(35,0,9,0x100));
        psxMemWrite32(0x10,I(9,9,10,1));
        psxMemWrite32(0x14,I(expected?5:4,10,0,2));
        psxMemWrite32(0x18,I(9,0,11,7));
        psxMemWrite32(0x1c,I(9,0,12,3));
        psxRegs->pc=0;
        psxInt.ExecuteBlock();
        require(psxRegs->GPR.r[8]==value,"lui/ori");
        require(psxMemRead32(0x100)==value,"store");
        require(psxRegs->GPR.r[9]==value,"load");
        require(psxRegs->GPR.r[10]==expected,"addiu");
        require(psxRegs->GPR.r[11]==7u,"branch delay");
        require(psxRegs->pc==0x20u,"taken branch PC");
        require(psxRegs->GPR.r[0]==0u,"zero register");
    }
    // Non-taken branch: dispatch must leave PC unchanged; ExecuteBlock
    // intentionally does not terminate until a later taken branch.
    psxRegs->GPR.r[10]=0;
    psxRegs->GPR.r[0]=0;
    psxRegs->pc=0x18;
    psxRegs->code=I(5,10,0,2); // bne zero,zero (not taken)
    psxBNE();
    require(psxRegs->pc==0x18,"non-taken BNE keeps sequential PC");
    psxInt.Shutdown();
    psxMemShutdown();
    std::printf("PSX4ALL Stage14 MIPS interpreter: %u randomized programs, %u GPR/RAM/branch assertions PASS\n",cases,cases*7u);
    return 0;
}
