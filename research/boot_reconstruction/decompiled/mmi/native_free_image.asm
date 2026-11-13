002dda44 cpy r12,sp
002dda48 stmdb sp!,{r4,r5,r11,r12,lr,pc}
002dda4c sub r11,r12,#0x4
002dda50 and r4,r0,#0xff
002dda54 cmp r4,#0x1
002dda58 bls 0x002dda6c
002dda5c ldr r3,[0x2ddb70]
002dda60 ldrh r3,[r3,#0x0]
002dda64 cmp r3,r4
002dda68 bhi 0x002dda78
002dda6c mov r0,#0x400
002dda70 add r0,r0,#0x6
002dda74 ldmia sp,{r4,r5,r11,sp,pc}
002dda78 add r2,r4,r4, lsl #0x3
002dda7c ldr r3,[0x2ddb74]
002dda80 ldr r3,[r3,#0x0]
002dda84 add r5,r3,r2, lsl #0x2
002dda88 ldrb r3,[r5,#-0x24]!
002dda8c cmp r3,#0x0
002dda90 moveq r0,#0x400
002dda94 addeq r0,r0,#0x6
002dda98 ldmiaeq sp,{r4,r5,r11,sp,pc}
002dda9c ldrb r3,[r5,#0x1b]
002ddaa0 cmp r3,#0x1
002ddaa4 bne 0x002ddb38
002ddaa8 ldr r0,[r5,#0xc]
002ddaac cmp r0,#0x0
002ddab0 blne 0x002995d0
002ddab4 ldr r3,[r5,#0x14]
002ddab8 cmp r3,#0x0
002ddabc beq 0x002ddb38
002ddac0 mov r1,#0x1
002ddac4 ldr r3,[0x2ddb70]
002ddac8 ldrh r3,[r3,#0x0]
002ddacc cmp r3,r1
002ddad0 bls 0x002ddb1c
002ddad4 sub r0,r4,#0x1
002ddad8 ldr lr,[0x2ddb74]
002ddadc ldr r3,[0x2ddb70]
002ddae0 ldrh r12,[r3,#0x0]
002ddae4 cmp r1,r0
002ddae8 beq 0x002ddb08
002ddaec ldr r3,[lr,#0x0]
002ddaf0 add r2,r1,r1, lsl #0x3
002ddaf4 add r3,r3,r2, lsl #0x2
002ddaf8 ldr r2,[r5,#0x14]
002ddafc ldr r3,[r3,#0x14]
002ddb00 cmp r2,r3
002ddb04 beq 0x002ddb1c
002ddb08 add r3,r1,#0x1
002ddb0c mov r3,r3, lsl #0x10
002ddb10 mov r1,r3, lsr #0x10
002ddb14 cmp r12,r1
002ddb18 bhi 0x002ddae4
002ddb1c ldr r3,[0x2ddb70]
002ddb20 ldrh r3,[r3,#0x0]
002ddb24 cmp r3,r1
002ddb28 bne 0x002ddb38
002ddb2c ldr r0,[r5,#0x14]
002ddb30 cmp r0,#0x0
002ddb34 blne 0x002995d0
002ddb38 mov r2,#0x0
002ddb3c cpy r3,r5
002ddb40 str r2,[r3],#0x4
002ddb44 str r2,[r5,#0x4]
002ddb48 add r3,r3,#0x4
002ddb4c str r2,[r3],#0x4
002ddb50 str r2,[r3],#0x4
002ddb54 str r2,[r3],#0x4
002ddb58 str r2,[r3],#0x4
002ddb5c str r2,[r3],#0x4
002ddb60 str r2,[r3],#0x4
002ddb64 str r2,[r3,#0x0]
002ddb68 cpy r0,r2
002ddb6c ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002d3320 DATA
