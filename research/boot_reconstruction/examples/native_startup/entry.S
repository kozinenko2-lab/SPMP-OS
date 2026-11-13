/* New SPMP-OS research startup, reconstructed from the observed calling contract.
 * No OEM code copied. No SP replacement, IRQ/CP15/MMIO or native API call.
 */
.syntax unified
.arch armv5te
.arm
.section .text.spmp_start,"ax",%progbits
.balign 4
.global _spmp_start
.type _spmp_start,%function
_spmp_start:
    stmdb sp!, {r4-r11, lr}
    mov r4, sp                  /* Recover this exact frame after C returns. */
    bic sp, sp, #7              /* New C uses AAPCS 8-byte stack alignment. */
    mov r5, r0                  /* r1-r3 are deliberately not launch inputs. */
#ifdef SPMP_NATIVE_PREFLIGHT
    mov r0, r5
    bl spmp_native_preflight
    cmp r0, #0
    bne 2f
#endif
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
    mov r3, #0
1:
    cmp r1, r2
    strlo r3, [r1], #4
    blo 1b
    mov r0, r5
    bl spmp_app_main
2:
    mov sp, r4
    ldmia sp!, {r4-r11, lr}
    bx lr
.size _spmp_start, .-_spmp_start
.ltorg
.section .note.GNU-stack,"",%progbits
