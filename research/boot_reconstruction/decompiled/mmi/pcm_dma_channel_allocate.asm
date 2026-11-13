002a2e88 cpy r12,sp
002a2e8c stmdb sp!,{r4,r11,r12,lr,pc}
002a2e90 sub r11,r12,#0x4
002a2e94 ldr r0,[0x2a2f0c]
002a2e98 bl 0x0033a048
002a2e9c ldr r3,[0x2a2f10]
002a2ea0 ldr r3,[r3,#0x0]
002a2ea4 cmp r3,#0x0
002a2ea8 bne 0x002a2ec4
002a2eac ldr r4,[0x2a2f10]
002a2eb0 ldr r0,[0x2a2f14]
002a2eb4 bl 0x0033a0c0
002a2eb8 ldr r3,[r4,#0x0]
002a2ebc cmp r3,#0x0
002a2ec0 beq 0x002a2eb0
002a2ec4 mov r4,#0x0
002a2ec8 mov r2,#0x1
002a2ecc ldr r3,[0x2a2f10]
002a2ed0 ldr r3,[r3,#0x0]
002a2ed4 ands r1,r3,r2, lsl r4
002a2ed8 bne 0x002a2ee8
002a2edc add r4,r4,#0x1
002a2ee0 cmp r4,#0x1
002a2ee4 bls 0x002a2ed4
002a2ee8 ldr r3,[0x2a2f10]
002a2eec ldr r2,[r3,#0x0]
002a2ef0 mov r1,#0x1
002a2ef4 bic r2,r2,r1, lsl r4
002a2ef8 str r2,[r3,#0x0]
002a2efc ldr r0,[0x2a2f0c]
002a2f00 bl 0x0033a05c
002a2f04 cpy r0,r4
002a2f08 ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
0029293c UNCONDITIONAL_CALL
00293cb0 UNCONDITIONAL_CALL
