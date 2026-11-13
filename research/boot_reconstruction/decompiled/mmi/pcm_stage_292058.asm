00292058 cpy r12,sp
0029205c stmdb sp!,{r11,r12,lr,pc}
00292060 sub r11,r12,#0x4
00292064 subs r1,r0,#0x0
00292068 ldrgt r3,[0x292094]
0029206c strgt r1,[r3,#0x0]
00292070 ldmiagt sp,{r11,sp,pc}
00292074 ldr r3,[0x292098]
00292078 ldr r3,[r3,#0x0]
0029207c ldr r3,[r3,#0x0]
00292080 tst r3,#0x10
00292084 ldmiaeq sp,{r11,sp,pc}
00292088 ldr r0,[0x29209c]
0029208c bl 0x00298b64
00292090 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002c55cc UNCONDITIONAL_CALL
