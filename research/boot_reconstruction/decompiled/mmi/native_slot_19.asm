002ddeec cpy r12,sp
002ddef0 stmdb sp!,{r4,r5,r11,r12,lr,pc}
002ddef4 sub r11,r12,#0x4
002ddef8 mov r1,r1, lsl #0x10
002ddefc mov r5,r1, lsr #0x10
002ddf00 subs r4,r0,#0x0
002ddf04 ldreq r0,[0x2ddf28]
002ddf08 cpyeq r1,r5
002ddf0c bleq 0x00312ed8
002ddf10 cmp r4,#0x1
002ddf14 ldreq r0,[0x2ddf28]
002ddf18 cpyeq r1,r5
002ddf1c bleq 0x00312f00
002ddf20 mov r0,#0x0
002ddf24 ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002d3388 DATA
