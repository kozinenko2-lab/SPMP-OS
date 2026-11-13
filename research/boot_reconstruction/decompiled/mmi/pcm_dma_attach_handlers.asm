002a2f64 cpy r12,sp
002a2f68 stmdb sp!,{r4,r5,r6,r11,r12,lr,pc}
002a2f6c sub r11,r12,#0x4
002a2f70 sub sp,sp,#0xc
002a2f74 cpy r3,r1
002a2f78 add r6,r0,#0x18
002a2f7c str r2,[sp,#0x0]
002a2f80 mov r5,r0, lsl #0x2
002a2f84 ldr r4,[0x2a2fc8]
002a2f88 add r2,r5,r4
002a2f8c str r2,[sp,#0x4]
002a2f90 rsb r0,r0,r0, lsl #0x3
002a2f94 ldr r2,[0x2a2fcc]
002a2f98 add r2,r2,r0, lsl #0x2
002a2f9c str r2,[sp,#0x8]
002a2fa0 cpy r0,r6
002a2fa4 mov r1,#0x2
002a2fa8 mov r2,#0x0
002a2fac bl 0x00339c10
002a2fb0 ldr r0,[r5,r4]
002a2fb4 bl 0x00339c74
002a2fb8 cpy r0,r6
002a2fbc bl 0x00339cd8
002a2fc0 sub sp,r11,#0x18
002a2fc4 ldmia sp,{r4,r5,r6,r11,sp,pc}
REFERENCES TO ENTRY:
00292970 UNCONDITIONAL_CALL
00293ce4 UNCONDITIONAL_CALL
