00040f08 stmdb sp!,{r4,r5,r6,r7,lr}
00040f0c cpy r4,r0
00040f10 cpy r6,r1
00040f14 ldr r1,[r0,#0x24]
00040f18 bl 0x000409e4
00040f1c cpy r5,r0
00040f20 cpy r0,r4
00040f24 ldr r1,[r4,#0x28]
00040f28 bl 0x000409e4
00040f2c ldr r3,[r5,r6,lsl #0x4]
00040f30 add r7,r0,r3
00040f34 ldr r3,[0x40f90]
00040f38 ldr r6,[r3,#0x0]
00040f3c cmp r6,#0x0
00040f40 beq 0x00040f7c
00040f44 ldr r5,[0x40f94]
00040f48 mov r4,#0x0
00040f4c cpy r0,r7
00040f50 ldr r1,[r5,#0x0]
00040f54 bl 0x0004caf4
00040f58 cmp r0,#0x0
00040f5c bne 0x00040f68
00040f60 ldr r0,[r5,#0x4]
00040f64 ldmia sp!,{r4,r5,r6,r7,pc}
00040f68 add r3,r4,#0x1
00040f6c and r4,r3,#0xff
00040f70 cmp r4,r6
00040f74 addcc r5,r5,#0x8
00040f78 bcc 0x00040f4c
00040f7c ldr r0,[0x40f98]
00040f80 cpy r1,r7
00040f84 bl 0x0004563c
00040f88 mov r0,#0x0
00040f8c ldmia sp!,{r4,r5,r6,r7,pc}
REFERENCES TO ENTRY:
00041000 UNCONDITIONAL_CALL
00041020 UNCONDITIONAL_CALL
