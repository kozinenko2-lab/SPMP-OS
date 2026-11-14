; Exact-build excerpts, 8000_MMI.RAP SHA256:
; 5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e

; Ready wait: read -> delay -> compare; argument N yields N+1 reads.
00348e04  e5964000  ldr     r4,[r6,#0x0]       ; 0x9053200c
00348e0c  e3a0000a  mov     r0,#0xa
00348e10  ebff8374  bl      0x00339be8
00348e18  e1540003  cmp     r4,r3              ; 0x43585244 'CXRD'
00348e40  e2455001  sub     r5,r5,#0x1
00348e44  e3750001  cmn     r5,#0x1

; Incoming buffer: no capacity comparison exists before payload copy.
0034a258  e5931000  ldr     r1,[r3,#0x0]       ; CEVA ptr from 0x90532040
0034a26c  e2811201  add     r1,r1,#0x10000000
0034a274  ebfd3a7c  bl      0x00298c6c          ; copy four-byte header
0034a288  e1d440b2  ldrh    r4,[r4,#0x2]       ; payload_bytes
0034a298  e2811241  add     r1,r1,#0x10000004
0034a2a0  ebfd3a71  bl      0x00298c6c          ; unchecked payload copy
0034a3e0  00640c60  .word   0x00640c60          ; buffer
0034a528  00640d60  .word   0x00640d60          ; next RTOS/IRQ object

; Scale command executes before or after reply according to request +0x30.
00349bd4  e5906030  ldr     r6,[r0,#0x30]
00349be0  ebfe72b6  bl      0x002e66c0          ; wait scaler idle
00349c24  ebfffee5  bl      0x003497c0          ; pre-reply path
00349c60  e3560000  cmp     r6,#0x0
00349c9c  ebfffec7  bl      0x003497c0          ; post-reply path

; CEVA allocation/free use the normal OEM heap wrappers.
00349cdc  e5900004  ldr     r0,[r0,#0x4]        ; requested bytes
00349d00  ebfd3e20  bl      0x00299588
00349d04  e5840004  str     r0,[r4,#0x4]        ; response pointer
00349d48  e5906004  ldr     r6,[r0,#0x4]
00349d78  ebfd3e14  bl      0x002995d0

; ARM926 publication barrier used by response path.
00349920  e10f2000  mrs     r2,cpsr
00349928  e38440c0  orr     r4,r4,#0xc0         ; mask IRQ/FIQ
00349930  ee17ff7a  mrc     p15,0,pc,c7,c10,3   ; test/clean D-cache
00349934  1afffffd  bne     0x00349930
0034993c  ee077f9a  mcr     p15,0,r7,c7,c10,4   ; drain write buffer
00349950  e129f003  msr     cpsr_cf,r3
00349960  e5832000  str     r2,[r3,#0x0]        ; publish 0x90532034
