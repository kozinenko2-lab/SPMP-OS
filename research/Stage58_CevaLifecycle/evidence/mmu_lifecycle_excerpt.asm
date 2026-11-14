; Exact-build excerpts only; no original binary is included.
; RedBoot page-table segment SHA256:
; 80a4560940f1313613a249ed01bce202961ad6ceeaa9809f3aa034f6d334e45b

; RedBoot family 1/2/4 installs TTBR 0x00060000.
0003d1e4  e59f3024  ldr r3,[0x0003d210]
0003d1e8  ee110f10  mrc p15,0,r0,c1,c0,0
0003d1ec  e3c00001  bic r0,r0,#1
0003d1f0  ee010f10  mcr p15,0,r0,c1,c0,0
0003d1f8  ee070f17  mcr p15,0,r0,c7,c7,0
0003d1fc  ee080f17  mcr p15,0,r0,c8,c7,0
0003d200  ee023f10  mcr p15,0,r3,c2,c0,0
0003d208  ee030f10  mcr p15,0,r0,c3,c0,0
0003d210  00060000  .word 0x00060000

; RedBoot family 3/5 has the same sequence at 0x0003b4d8.
0003b4d8  e59f3024  ldr r3,[0x0003b504]
0003b4f4  ee023f10  mcr p15,0,r3,c2,c0,0
0003b504  00060000  .word 0x00060000

; Exact-build 8000_MMI.RAP SHA256:
; 5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e

; Close packet begins with opcode 0x84 and is submitted to the mailbox.
00348f80  e3e0307b  mvn r3,#0x7b
00348f84  e5c23000  strb r3,[r2,#0]
00348f88  e283307c  add r3,r3,#0x7c
00348f8c  e5c23001  strb r3,[r2,#1]
00348f90  e5c23002  strb r3,[r2,#2]
00348f94  e5c23003  strb r3,[r2,#3]
00348ffc  eb00040c  bl 0x0034a034

; Only the last reference closes and powers down the core.
0034935c  e3530001  cmp r3,#1
00349364  ebffff01  bl 0x00348f70
00349378  e3c33c01  bic r3,r3,#0x100
00349384  ebfd5bd1  bl 0x002a02d0
00349394  ebfd5bcd  bl 0x002a02d0
003493a0  e2433001  sub r3,r3,#1

; Reply structures are read through the uncached alias.
0034953c  e3851201  orr r1,r5,#0x10000000
00349540  e3a02060  mov r2,#0x60
00349544  ebfd3dc8  bl 0x00298c6c
