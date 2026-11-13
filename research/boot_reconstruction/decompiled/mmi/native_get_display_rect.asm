002de020 cpy r12,sp
002de024 stmdb sp!,{r4,r5,r11,r12,lr,pc}
002de028 sub r11,r12,#0x4
002de02c cpy r4,r0
002de030 mov r5,#0x0
002de034 strh r5,[r0,#0x0]
002de038 strh r5,[r0,#0x2]
002de03c ldr r3,[0x2de064]
002de040 mov lr,pc
002de044 ldr pc,[r3,#0x0]
002de048 strh r0,[r4,#0x4]
002de04c ldr r3,[0x2de068]
002de050 mov lr,pc
002de054 ldr pc,[r3,#0x0]
002de058 strh r0,[r4,#0x6]
002de05c cpy r0,r5
002de060 ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002dd250 UNCONDITIONAL_CALL
002d33a0 DATA
