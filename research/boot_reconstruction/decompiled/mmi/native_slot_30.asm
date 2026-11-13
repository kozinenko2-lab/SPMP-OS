002de69c cpy r12,sp
002de6a0 stmdb sp!,{r4,r5,r11,r12,lr,pc}
002de6a4 sub r11,r12,#0x4
002de6a8 cpy r4,r0
002de6ac cpy r5,r1
002de6b0 ldr r0,[0x2de6d0]
002de6b4 bl 0x00312d48
002de6b8 strb r0,[r4,#0x0]
002de6bc ldr r0,[0x2de6d4]
002de6c0 bl 0x00312d48
002de6c4 strb r0,[r5,#0x0]
002de6c8 mov r0,#0x0
002de6cc ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002d33e0 DATA
