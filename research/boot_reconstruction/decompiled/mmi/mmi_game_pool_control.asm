002e3898 cpy r12,sp
002e389c stmdb sp!,{r4,r5,r11,r12,lr,pc}
002e38a0 sub r11,r12,#0x4
002e38a4 cpy r5,r0
002e38a8 cpy r4,r1
002e38ac mov r3,#0xff00
002e38b0 add r3,r3,#0xff
002e38b4 cmp r1,r3
002e38b8 cmpeq r0,#0x1
002e38bc bne 0x002e38dc
002e38c0 ldr r3,[0x2e38fc]
002e38c4 ldr r0,[0x2e3900]
002e38c8 cpy r1,r3
002e38cc ldr r2,[0x2e3904]
002e38d0 rsb r2,r3,r2
002e38d4 mov r3,#0x8
002e38d8 bl 0x0032e594
002e38dc mov r3,#0xff00
002e38e0 add r3,r3,#0xff
002e38e4 cmp r4,r3
002e38e8 cmpeq r5,#0x0
002e38ec ldmiane sp,{r4,r5,r11,sp,pc}
002e38f0 ldr r0,[0x2e3900]
002e38f4 bl 0x0032e5a8
002e38f8 ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002e3920 UNCONDITIONAL_CALL
002e3940 UNCONDITIONAL_CALL
