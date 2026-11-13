000413d0 str lr,[sp,#-0x4]!
000413d4 cpy r12,r2
000413d8 sub r0,r0,#0x1
000413dc cmp r0,#0x1c
000413e0 ldrls pc,[pc,r0,lsl #0x2]
000413e4 b 0x000414ac
0004145c cpy lr,r1
00041460 ldr r3,[r1,#0x0]
00041464 bic r3,r3,#0xff000000
00041468 mov r0,r3, lsl #0x2
0004146c tst r0,#0x2000000
00041470 orrne r0,r0,#0xfc000000
00041474 ldr r2,[r1,#0x0]
00041478 and r2,r2,#0xff000000
0004147c rsb r3,r1,r12
00041480 add r3,r3,r0
00041484 mov r3,r3, lsl #0x6
00041488 orr r2,r2,r3, lsr #0x8
0004148c str r2,[r1,#0x0]
00041490 mov r0,#0x0
00041494 ldr pc,[sp],#0x4
00041498 ldr r3,[r1,#0x0]
0004149c add r3,r3,r2
000414a0 str r3,[r1,#0x0]
000414a4 mov r0,#0x0
000414a8 ldr pc,[sp],#0x4
000414ac ldr r0,[0x414bc]
000414b0 bl 0x0004563c
000414b4 mvn r0,#0x0
000414b8 ldr pc,[sp],#0x4
REFERENCES TO ENTRY:
000410e4 UNCONDITIONAL_CALL
