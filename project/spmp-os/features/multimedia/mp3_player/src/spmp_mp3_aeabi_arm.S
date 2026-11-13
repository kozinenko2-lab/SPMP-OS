/* ARMv5 AAPCS divmod veneers, GPL-2.0-or-later. */
.syntax unified
.arm
.text
.align 2
.global __aeabi_uidivmod
.type __aeabi_uidivmod,%function
__aeabi_uidivmod:
 push {r4,r5,r6,lr}
 mov r4,r0
 mov r5,r1
 bl __aeabi_uidiv
 mul r6,r0,r5
 sub r1,r4,r6
 pop {r4,r5,r6,pc}
.size __aeabi_uidivmod,.-__aeabi_uidivmod
.global __aeabi_idivmod
.type __aeabi_idivmod,%function
__aeabi_idivmod:
 push {r4,r5,r6,lr}
 mov r4,r0
 mov r5,r1
 bl __aeabi_idiv
 mul r6,r0,r5
 sub r1,r4,r6
 pop {r4,r5,r6,pc}
.size __aeabi_idivmod,.-__aeabi_idivmod
.global __aeabi_uldivmod
.type __aeabi_uldivmod,%function
__aeabi_uldivmod:
 push {r4,lr}
 sub sp,sp,#24
 add r4,sp,#8
 str r4,[sp]
 bl spmp_udivmod64
 ldr r0,[sp,#8]
 ldr r1,[sp,#12]
 ldr r2,[sp,#16]
 ldr r3,[sp,#20]
 add sp,sp,#24
 pop {r4,pc}
.size __aeabi_uldivmod,.-__aeabi_uldivmod
