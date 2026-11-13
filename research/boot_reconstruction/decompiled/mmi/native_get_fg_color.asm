002ddea0 cpy r12,sp
002ddea4 stmdb sp!,{r4,r11,r12,lr,pc}
002ddea8 sub r11,r12,#0x4
002ddeac cpy r4,r0
002ddeb0 ldr r0,[0x2ddee8]
002ddeb4 bl 0x00312d20
002ddeb8 mov r0,r0, lsl #0x10
002ddebc mov r0,r0, lsr #0x10
002ddec0 and r3,r0,#0xf800
002ddec4 mov r3,r3, asr #0x8
002ddec8 strb r3,[r4,#0x0]
002ddecc and r3,r0,#0x7e0
002dded0 mov r3,r3, asr #0x3
002dded4 strb r3,[r4,#0x1]
002dded8 mov r0,r0, lsl #0x3
002ddedc strb r0,[r4,#0x2]
002ddee0 mov r0,#0x0
002ddee4 ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
002d3380 DATA
