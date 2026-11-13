#include "../common.h"
#ifdef SPMP_PSX4ALL
#include "../../spmp_port/spmp_psx_opt.h"
#endif

u32 psxBranchTest_rec(u32 cycles, u32 pc)
{
	/* Misc helper */
	psxRegs->pc = pc;
	#ifdef SPMP_PSX4ALL
    const u32 elapsed = spmp_psx_scaled_cycles(cycles, (u32)BIAS_CYCLE_INC);
#else
    const u32 elapsed = (u32)((float)cycles * BIAS_CYCLE_INC);
#endif
    psxRegs->cycle += elapsed;

	/* Make sure interrupts  always when mcd is active */
	//if( mcdst != 0 || (psxRegs->cycle - psxRegs->psx_next_io_base)  >= psxRegs->psx_next_io_count )
	{
		update_hw(elapsed);
	}

	u32 compiledpc = (u32)PC_REC32(psxRegs->pc);
	if( compiledpc != 0 )
	{
		return compiledpc;
	}
	return recRecompile();
}

#ifdef IPHONE
extern "C" void sys_icache_invalidate(const void* Addr, size_t len);

void clear_insn_cache(u32 start, u32 end, int type)
{
	sys_icache_invalidate((void*)start, end - start);
}
#endif
