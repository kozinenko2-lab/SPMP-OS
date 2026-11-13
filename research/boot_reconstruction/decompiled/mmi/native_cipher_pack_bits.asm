00463254 cpy r12,sp
00463258 stmdb sp!,{r4,r5,r6,r7,r11,r12,lr,pc}
0046325c sub r11,r12,#0x4
00463260 cpy r6,r0
00463264 cpy r7,r1
00463268 cpy r5,r2
0046326c mov r4,#0x0
00463270 cpy r1,r4
00463274 mov r2,r2, asr #0x3
00463278 bl 0x00298d34
0046327c cmp r4,r5
00463280 ldmiage sp,{r4,r5,r6,r7,r11,sp,pc}
00463284 mov r0,r4, asr #0x3
00463288 ldrb r1,[r7,r4]
0046328c and r2,r4,#0x7
00463290 ldrb r3,[r0,r6]
00463294 orr r3,r3,r1, lsl r2
00463298 strb r3,[r0,r6]
0046329c add r4,r4,#0x1
004632a0 cmp r4,r5
004632a4 blt 0x00463284
004632a8 ldmia sp,{r4,r5,r6,r7,r11,sp,pc}
REFERENCES TO ENTRY:
00462f88 UNCONDITIONAL_CALL
