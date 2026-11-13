/* AAPCS: __aeabi_uidivmod returns quotient in r0, remainder in r1. */
.syntax unified
.arm
.text
.align 2
.global __aeabi_uidivmod
.type __aeabi_uidivmod,%function
__aeabi_uidivmod:
  push {r4,lr}
  sub sp,sp,#8
  mov r2,sp
  bl spmp_udivmod32
  ldr r1,[sp]
  add sp,sp,#8
  pop {r4,pc}
.size __aeabi_uidivmod, .-__aeabi_uidivmod
