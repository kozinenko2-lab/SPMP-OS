/* AAPCS __aeabi_uldivmod ABI: input n in r0:r1, d in r2:r3;
 * quotient in r0:r1, remainder in r2:r3. ARMv5 compatible. */
.syntax unified
.arm
.text
.align 2
.global __aeabi_uldivmod
.type __aeabi_uldivmod,%function
__aeabi_uldivmod:
    push {r4,lr}
    sub sp,sp,#16
    add r4,sp,#8
    str r4,[sp]              @ 5th C argument: uint64_t *remainder
    bl spmp_udivmod64
    ldr r2,[sp,#8]
    ldr r3,[sp,#12]
    add sp,sp,#16
    pop {r4,pc}
.size __aeabi_uldivmod,.-__aeabi_uldivmod
