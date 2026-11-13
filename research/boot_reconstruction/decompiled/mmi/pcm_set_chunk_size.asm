00294878 cpy r12,sp
0029487c stmdb sp!,{r11,r12,lr,pc}
00294880 sub r11,r12,#0x4
00294884 subs lr,r0,#0x0
00294888 bgt 0x002948a0
0029488c ldr r0,[0x2948e8]
00294890 cpy r1,lr
00294894 bl 0x00298b64
00294898 mvn r0,#0x0
0029489c ldmia sp,{r11,sp,pc}
002948a0 ldr r3,[0x2948ec]
002948a4 smull r1,r2,r3,lr
002948a8 mov r3,lr, asr #0x1f
002948ac rsb r12,r3,r2, asr #0x4
002948b0 add r3,r12,r12, lsl #0x2
002948b4 rsb r3,r3,r3, lsl #0x4
002948b8 mov r12,r3, lsl #0x2
002948bc ldr r3,[0x2948f0]
002948c0 ldr r1,[r3,#0x0]
002948c4 rsb r1,r12,r1
002948c8 ldr r3,[0x2948f4]
002948cc str r1,[r3,#0x0]
002948d0 ldr r0,[0x2948f8]
002948d4 cpy r2,lr
002948d8 cpy r3,r12
002948dc bl 0x00298b64
002948e0 mov r0,#0x0
002948e4 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002c55c4 UNCONDITIONAL_CALL
