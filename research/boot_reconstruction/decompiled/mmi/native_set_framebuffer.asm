002de06c cpy r12,sp
002de070 stmdb sp!,{r4,r5,r6,r11,r12,lr,pc}
002de074 sub r11,r12,#0x4
002de078 mov r0,r0, lsl #0x10
002de07c mov r1,r1, lsl #0x10
002de080 mov r1,r1, lsr #0x10
002de084 mov r3,#0x3fc
002de088 add r3,r3,#0x3
002de08c cmp r3,r0, lsr #0x10
002de090 movcc r0,#0x400
002de094 addcc r0,r0,#0x70000001
002de098 ldmiacc sp,{r4,r5,r6,r11,sp,pc}
002de09c cmp r1,#0x400
002de0a0 movcs r0,#0x400
002de0a4 addcs r0,r0,#0x70000001
002de0a8 ldmiacs sp,{r4,r5,r6,r11,sp,pc}
002de0ac ldr r3,[0x2de114]
002de0b0 mov lr,pc
002de0b4 ldr pc,[r3,#0x0]
002de0b8 cpy r5,r0
002de0bc ldr r3,[0x2de118]
002de0c0 mov lr,pc
002de0c4 ldr pc,[r3,#0x0]
002de0c8 mul r5,r0,r5
002de0cc mov r5,r5, lsl #0x1
002de0d0 ldr r3,[0x2de11c]
002de0d4 mov lr,pc
002de0d8 ldr pc,[r3,#0x0]
002de0dc cpy r4,r0
002de0e0 ldr r6,[0x2de120]
002de0e4 mov lr,pc
002de0e8 ldr pc,[r6,#0x0]
002de0ec cmp r4,r0
002de0f0 beq 0x002de108
002de0f4 mov lr,pc
002de0f8 ldr pc,[r6,#0x0]
002de0fc mov r1,#0x0
002de100 cpy r2,r5
002de104 bl 0x00298d34
002de108 bl 0x0029e450
002de10c mov r0,#0x0
002de110 ldmia sp,{r4,r5,r6,r11,sp,pc}
REFERENCES TO ENTRY:
002d3410 DATA
