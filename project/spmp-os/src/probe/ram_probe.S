/* DefenderOS Stage 1, ARMv5TE read-only processor status probe.
 * NOT a boot image. Do not run before verifying RAM map and entry ABI.
 * Uses no stack, MMIO writes, SVC, cache changes, interrupts or NAND calls.
 * Saves to its own linked .data; assumes caller uses BL/BLX with valid LR.
 */
.syntax unified
.cpu arm926ej-s
.arm
.section .text.probe,"ax",%progbits
.balign 4
.global defos_probe_entry
.type defos_probe_entry,%function
defos_probe_entry:
    ldr r0, =defos_probe_data
    ldr r1, =0x31534F44      /* 'DOS1' little endian */
    str r1, [r0, #0]
    mrc p15, 0, r1, c0, c0, 0  /* CP15 Main ID register */
    str r1, [r0, #4]
    mrs r1, cpsr
    str r1, [r0, #8]
    mrc p15, 0, r1, c1, c0, 0  /* control register (read only) */
    str r1, [r0, #12]
    str sp, [r0, #16]
    str lr, [r0, #20]
    bx lr
.size defos_probe_entry, .-defos_probe_entry
.section .data.probe,"aw",%progbits
.balign 4
.global defos_probe_data
defos_probe_data:
    .word 0x00000000 /* status marker */
    .word 0x00000000 /* CPU ID */
    .word 0x00000000 /* CPSR */
    .word 0x00000000 /* CP15 Control */
    .word 0x00000000 /* SP at invocation */
    .word 0x00000000 /* LR at invocation */
