002dde4c cpy r12,sp
002dde50 stmdb sp!,{r11,r12,lr,pc}
002dde54 sub r11,r12,#0x4
002dde58 ldrb r3,[r0,#0x0]
002dde5c and r3,r3,#0xf8
002dde60 ldrb r1,[r0,#0x1]
002dde64 and r1,r1,#0xfc
002dde68 mov r1,r1, lsl #0x3
002dde6c add r1,r1,r3, lsl #0x8
002dde70 ldrb r3,[r0,#0x2]
002dde74 add r1,r1,r3, lsr #0x3
002dde78 mov r1,r1, lsl #0x10
002dde7c mov r1,r1, lsr #0x10
002dde80 ldr r3,[0x2dde98]
002dde84 str r1,[r3,#0x0]
002dde88 ldr r0,[0x2dde9c]
002dde8c bl 0x00312d18
002dde90 mov r0,#0x0
002dde94 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002d3378 DATA
