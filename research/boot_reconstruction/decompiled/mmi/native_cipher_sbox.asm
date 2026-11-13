004630b4 cpy r12,sp
004630b8 stmdb sp!,{r4,r5,r6,r11,r12,lr,pc}
004630bc sub r11,r12,#0x4
004630c0 cpy r5,r0
004630c4 cpy r4,r1
004630c8 mov r6,#0x0
004630cc ldrb r3,[r4,#0x0]
004630d0 ldrb r2,[r4,#0x5]
004630d4 add r2,r2,r3, lsl #0x1
004630d8 and r2,r2,#0xff
004630dc ldrb r3,[r4,#0x1]
004630e0 ldrb r1,[r4,#0x2]
004630e4 mov r1,r1, lsl #0x2
004630e8 add r1,r1,r3, lsl #0x3
004630ec ldrb r3,[r4,#0x3]
004630f0 add r1,r1,r3, lsl #0x1
004630f4 ldrb r3,[r4,#0x4]
004630f8 add r1,r1,r3
004630fc and r1,r1,#0xff
00463100 add r2,r2,r6, lsl #0x2
00463104 add r1,r1,r2, lsl #0x4
00463108 cpy r0,r5
0046310c ldr r3,[0x463138]
00463110 add r1,r1,r3
00463114 mov r2,#0x4
00463118 bl 0x0046321c
0046311c add r3,r6,#0x1
00463120 and r6,r3,#0xff
00463124 add r4,r4,#0x6
00463128 add r5,r5,#0x4
0046312c cmp r6,#0x7
00463130 bls 0x004630cc
00463134 ldmia sp,{r4,r5,r6,r11,sp,pc}
REFERENCES TO ENTRY:
0046308c UNCONDITIONAL_CALL
