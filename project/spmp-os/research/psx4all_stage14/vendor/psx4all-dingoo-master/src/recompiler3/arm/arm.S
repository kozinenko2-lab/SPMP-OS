.text
.global clear_insn_cache
.type	clear_insn_cache, %function
.align 4

clear_insn_cache:
	#ifdef SPMP_PSX4ALL
    b spmp_psx_icache_sync
#else
    swi 0x9f0002
#endif
	bx lr
