; Exact-build excerpt from Ghidra 12.2 listing.
; 8000_MMI.RAP SHA256 5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e
; Research evidence only; this is not a portable SPMP-OS driver.

00349f54  ebfffff7  bl      0x00349f38      ; read 0x9053204c
00349f60  e3100008  tst     r0,#0x8
00349f78  e3a00008  mov     r0,#0x8
00349f7c  ebffffe9  bl      0x00349f28      ; acknowledge reply
00349f88  e3130010  tst     r3,#0x10
00349fa0  e3a00010  mov     r0,#0x10
00349fa4  ebffffdf  bl      0x00349f28      ; acknowledge command
00349fa8  e3a00002  mov     r0,#0x2         ; request DSR

0034a31c  e59f00bc  ldr     r0,[0x34a3e0]  ; command buffer 0x00640c60
0034a320  ebffffbf  bl      0x0034a224      ; copy header/payload
0034a358  e59f0080  ldr     r0,[0x34a3e0]
0034a35c  e5d02000  ldrb    r2,[r0,#0x0]    ; request opcode
0034a360  e59f3084  ldr     r3,[0x34a3ec]  ; table 0x0055b7e0
0034a364  e7933102  ldr     r3,[r3,r2,lsl #0x2]
0034a368  e3530000  cmp     r3,#0x0
0034a374  e1a0e00f  mov     lr,pc
0034a378  e1a0f003  mov     pc,r3           ; handler(message,total_bytes)

; Unknown opcode, when no_reply == 0: four-byte error response.
0034a3a0  e5c45000  strb    r5,[r4,#0x0]    ; payload bytes = 0
0034a3a4  e5c46001  strb    r6,[r4,#0x1]    ; echoed opcode
0034a3a8  e3a03003  mov     r3,#0x3
0034a3ac  e5c43002  strb    r3,[r4,#0x2]    ; error status = 3
0034a3b0  e5c45003  strb    r5,[r4,#0x3]

; PIU setup: vector 31, priority 12, ISR/DSR and dispatcher thread.
0034a474  e3a0001f  mov     r0,#0x1f
0034a478  e3a0100c  mov     r1,#0xc
0034a480  e59f30a4  ldr     r3,[0x34a52c]  ; 0x00349f48 ISR
0034a484  ebffbde1  bl      0x00339c10
0034a4a8  e3a03a02  mov     r3,#0x2000      ; stack bytes
0034a4c0  e3a00008  mov     r0,#0x8         ; thread priority
0034a4c4  e59f1078  ldr     r1,[0x34a544]  ; 0x0034a2c4
0034a4d0  ebffbd75  bl      0x00339aac

; Sparse 256-entry callback table (all other slots are zero).
0055b8e0  003499f8  .word   0x003499f8      ; opcode 0x40 LCD geometry
0055b8e4  00349b7c  .word   0x00349b7c      ; opcode 0x41 scale engine
0055b8e8  00349974  .word   0x00349974      ; opcode 0x42 LCD clear
0055b8ec  00349cc8  .word   0x00349cc8      ; opcode 0x43 malloc
0055b8f0  00349d34  .word   0x00349d34      ; opcode 0x44 free
0055b8f4  00349db8  .word   0x00349db8      ; opcode 0x45 debug message
0055b8f8  00349e3c  .word   0x00349e3c      ; opcode 0x46 debug flags
