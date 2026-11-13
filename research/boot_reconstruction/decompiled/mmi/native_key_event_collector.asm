002d2e6c cpy r12,sp
002d2e70 stmdb sp!,{r4,r5,r11,r12,lr,pc}
002d2e74 sub r11,r12,#0x4
002d2e78 bl 0x0045ffbc
002d2e7c ldr r2,[0x2d2f44]
002d2e80 ldr r3,[0x2d2f48]
002d2e84 cmp r0,#0x5
002d2e88 cpyne r5,r3
002d2e8c cpyeq r5,r2
002d2e90 mov r4,#0x0
002d2e94 bl 0x002d8b78
002d2e98 cmp r0,r4
002d2e9c beq 0x002d2eb8
002d2ea0 ldr r3,[0x2d2f4c]
002d2ea4 str r4,[r3,#0x0]
002d2ea8 ldr r3,[0x2d2f50]
002d2eac str r4,[r3,#0x0]
002d2eb0 cpy r0,r4
002d2eb4 ldmia sp,{r4,r5,r11,sp,pc}
002d2eb8 ldr r0,[0x2d2f4c]
002d2ebc ldr r3,[r0,#0x0]
002d2ec0 cmp r3,#0x0
002d2ec4 beq 0x002d2ef4
002d2ec8 cpy r4,r3
002d2ecc ldr r1,[0x2d2f54]
002d2ed0 ldrh r2,[r1,#0x0]
002d2ed4 add r3,r2,#0x1
002d2ed8 strh r3,[r1,#0x0]
002d2edc cmp r2,#0x2
002d2ee0 movhi r3,#0x0
002d2ee4 strhi r3,[r0,#0x0]
002d2ee8 strhhi r3,[r1,#0x0]
002d2eec cpy r0,r4
002d2ef0 ldmia sp,{r4,r5,r11,sp,pc}
002d2ef4 ldr r3,[0x2d2f50]
002d2ef8 ldr r3,[r3,#0x0]
002d2efc cmp r3,#0x0
002d2f00 beq 0x002d2f3c
002d2f04 mov r2,#0x0
002d2f08 mov r0,#0x1
002d2f0c ldr r3,[0x2d2f50]
002d2f10 ldr r1,[r3,#0x0]
002d2f14 ands r3,r1,r0, lsl r2
002d2f18 addne r3,r5,r2, lsl #0x2
002d2f1c ldrne r3,[r3,#0x4]
002d2f20 orrne r4,r4,r3
002d2f24 add r3,r2,#0x1
002d2f28 mov r3,r3, lsl #0x10
002d2f2c mov r2,r3, lsr #0x10
002d2f30 cmp r2,#0xf
002d2f34 bls 0x002d2f14
002d2f38 bic r4,r4,#0x800
002d2f3c cpy r0,r4
002d2f40 ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002d2f68 UNCONDITIONAL_CALL
002d300c UNCONDITIONAL_CALL
