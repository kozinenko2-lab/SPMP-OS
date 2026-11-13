002a4180 cpy r12,sp
002a4184 stmdb sp!,{r11,r12,lr,pc}
002a4188 sub r11,r12,#0x4
002a418c bl 0x002e83e0
002a4190 ands r0,r0,#0xc0000000
002a4194 bne 0x002a41b4
002a4198 ldr r2,[0x2a4220]
002a419c ldr r3,[0x2a4224]
002a41a0 str r2,[r3,#0x0]
002a41a4 ldr r2,[0x2a4228]
002a41a8 ldr r3,[0x2a422c]
002a41ac str r2,[r3,#0x0]
002a41b0 ldmia sp,{r11,sp,pc}
002a41b4 cmp r0,#0x40000000
002a41b8 bne 0x002a41d8
002a41bc ldr r2,[0x2a4230]
002a41c0 ldr r3,[0x2a4224]
002a41c4 str r2,[r3,#0x0]
002a41c8 ldr r2,[0x2a4234]
002a41cc ldr r3,[0x2a422c]
002a41d0 str r2,[r3,#0x0]
002a41d4 ldmia sp,{r11,sp,pc}
002a41d8 cmp r0,#0x80000000
002a41dc bne 0x002a41fc
002a41e0 ldr r2,[0x2a4238]
002a41e4 ldr r3,[0x2a4224]
002a41e8 str r2,[r3,#0x0]
002a41ec ldr r2,[0x2a423c]
002a41f0 ldr r3,[0x2a422c]
002a41f4 str r2,[r3,#0x0]
002a41f8 ldmia sp,{r11,sp,pc}
002a41fc cmp r0,#0xc0000000
002a4200 ldmiane sp,{r11,sp,pc}
002a4204 ldr r2,[0x2a4240]
002a4208 ldr r3,[0x2a4224]
002a420c str r2,[r3,#0x0]
002a4210 ldr r2,[0x2a4244]
002a4214 ldr r3,[0x2a422c]
002a4218 str r2,[r3,#0x0]
002a421c ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002a4458 UNCONDITIONAL_CALL
002a4530 UNCONDITIONAL_CALL
002e96e0 UNCONDITIONAL_CALL
002e9f94 UNCONDITIONAL_CALL
