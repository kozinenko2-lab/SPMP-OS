002ddc40 cpy r12,sp
002ddc44 stmdb sp!,{r4,r5,r6,r11,r12,lr,pc}
002ddc48 sub r11,r12,#0x4
002ddc4c mov r0,#0x0
002ddc50 mov r2,#0x1
002ddc54 ldr r3,[0x2ddd38]
002ddc58 ldrh r3,[r3,#0x0]
002ddc5c cmp r3,r2
002ddc60 bls 0x002ddc90
002ddc64 ldr r3,[0x2ddd3c]
002ddc68 ldr r12,[r3,#0x0]
002ddc6c ldr r3,[0x2ddd38]
002ddc70 ldrh r1,[r3,#0x0]
002ddc74 add r3,r2,r2, lsl #0x3
002ddc78 ldrb r3,[r12,r3,lsl #0x2]
002ddc7c cmp r3,#0x0
002ddc80 beq 0x002ddcb4
002ddc84 add r2,r2,#0x1
002ddc88 cmp r1,r2
002ddc8c bhi 0x002ddc74
002ddc90 cmp r0,#0x0
002ddc94 ldmiane sp,{r4,r5,r6,r11,sp,pc}
002ddc98 ldr r3,[0x2ddd38]
002ddc9c ldrh r3,[r3,#0x0]
002ddca0 add r3,r3,#0x14
002ddca4 cmp r3,#0xfe
002ddca8 subgt r0,r0,#0x5
002ddcac ldmiagt sp,{r4,r5,r6,r11,sp,pc}
002ddcb0 b 0x002ddcbc
002ddcb4 and r0,r2,#0xff
002ddcb8 b 0x002ddc90
002ddcbc ldr r3,[0x2ddd38]
002ddcc0 ldrh r0,[r3,#0x0]
002ddcc4 add r0,r0,r0, lsl #0x3
002ddcc8 mov r0,r0, lsl #0x2
002ddccc add r0,r0,#0x2d0
002ddcd0 bl 0x00299588
002ddcd4 subs r6,r0,#0x0
002ddcd8 subeq r0,r0,#0x1
002ddcdc ldmiaeq sp,{r4,r5,r6,r11,sp,pc}
002ddce0 ldr r5,[0x2ddd38]
002ddce4 ldrh r2,[r5,#0x0]
002ddce8 add r2,r2,r2, lsl #0x3
002ddcec ldr r4,[0x2ddd3c]
002ddcf0 cpy r0,r6
002ddcf4 ldr r1,[r4,#0x0]
002ddcf8 mov r2,r2, lsl #0x2
002ddcfc bl 0x00298c6c
002ddd00 ldrh r0,[r5,#0x0]
002ddd04 add r0,r0,r0, lsl #0x3
002ddd08 add r0,r6,r0, lsl #0x2
002ddd0c mov r1,#0x0
002ddd10 mov r2,#0x2d0
002ddd14 bl 0x00298d34
002ddd18 ldr r0,[r4,#0x0]
002ddd1c bl 0x002995d0
002ddd20 str r6,[r4,#0x0]
002ddd24 ldrh r3,[r5,#0x0]
002ddd28 and r0,r3,#0xff
002ddd2c add r3,r3,#0x14
002ddd30 strh r3,[r5,#0x0]
002ddd34 ldmia sp,{r4,r5,r6,r11,sp,pc}
REFERENCES TO ENTRY:
002dd920 UNCONDITIONAL_CALL
