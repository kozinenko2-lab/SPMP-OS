002de168 cpy r12,sp
002de16c stmdb sp!,{r11,r12,lr,pc}
002de170 sub r11,r12,#0x4
002de174 ands r1,r0,#0xff
002de178 bne 0x002de188
002de17c ldr r0,[0x2de1a8]
002de180 bl 0x00312dcc
002de184 b 0x002de1a0
002de188 cmp r1,#0x1
002de18c movne r0,#0x400
002de190 addne r0,r0,#0x70000006
002de194 ldmiane sp,{r11,sp,pc}
002de198 ldr r0,[0x2de1a8]
002de19c bl 0x00312dcc
002de1a0 mov r0,#0x0
002de1a4 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002d3428 DATA
