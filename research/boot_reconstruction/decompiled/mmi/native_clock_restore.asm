004610c4 cpy r12,sp
004610c8 stmdb sp!,{r4,r5,r6,r11,r12,lr,pc}
004610cc sub r11,r12,#0x4
004610d0 ldr r6,[0x461128]
004610d4 ldr r3,[r6,#0x0]
004610d8 cmp r3,#0x0
004610dc ldmiaeq sp,{r4,r5,r6,r11,sp,pc}
004610e0 mrs r5,cpsr
004610e4 mrs r4,cpsr
004610e8 orr r4,r4,#0xc0
004610ec msr cpsr_cf,r4
004610f0 bl 0x00296af4
004610f4 sub r0,r0,#0x1
004610f8 bl 0x00296bc8
004610fc bl 0x00296d38
00461100 mrs r3,cpsr
00461104 and r4,r5,#0xc0
00461108 bic r3,r3,#0xc0
0046110c orr r3,r3,r4
00461110 msr cpsr_cf,r3
00461114 ldr r0,[0x46112c]
00461118 bl 0x00298b64
0046111c mov r3,#0x0
00461120 str r3,[r6,#0x0]
00461124 ldmia sp,{r4,r5,r6,r11,sp,pc}
REFERENCES TO ENTRY:
002d3f64 UNCONDITIONAL_CALL
00460ed0 UNCONDITIONAL_CALL
