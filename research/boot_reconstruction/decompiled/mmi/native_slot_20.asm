002ddf2c cpy r12,sp
002ddf30 stmdb sp!,{r4,r5,r11,r12,lr,pc}
002ddf34 sub r11,r12,#0x4
002ddf38 cpy r5,r1
002ddf3c subs r4,r0,#0x0
002ddf40 bne 0x002ddf50
002ddf44 ldr r0,[0x2ddf6c]
002ddf48 bl 0x00312ef8
002ddf4c strh r0,[r5,#0x0]
002ddf50 cmp r4,#0x1
002ddf54 bne 0x002ddf64
002ddf58 ldr r0,[0x2ddf6c]
002ddf5c bl 0x00312f20
002ddf60 strh r0,[r5,#0x0]
002ddf64 mov r0,#0x0
002ddf68 ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002d3390 DATA
