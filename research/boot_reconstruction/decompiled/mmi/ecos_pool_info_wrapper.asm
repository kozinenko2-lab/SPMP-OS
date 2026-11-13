0032e5e4 cpy r12,sp
0032e5e8 stmdb sp!,{r11,r12,lr,pc}
0032e5ec sub r11,r12,#0x4
0032e5f0 mov r1,r1, lsl #0x10
0032e5f4 mov r3,#0x0
0032e5f8 strb r3,[r2,#0x1c]
0032e5fc mov r1,r1, lsr #0x10
0032e600 bl 0x004ce8d0
0032e604 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002e3848 UNCONDITIONAL_CALL
