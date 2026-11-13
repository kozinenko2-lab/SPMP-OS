00299a98 cpy r12,sp
00299a9c stmdb sp!,{r4,r5,r11,r12,lr,pc}
00299aa0 sub r11,r12,#0x4
00299aa4 ldr r4,[0x299acc]
00299aa8 ldr r3,[0x299ad0]
00299aac cmp r4,r3
00299ab0 ldmiacc sp,{r4,r5,r11,sp,pc}
00299ab4 cpy r5,r3
00299ab8 mov lr,pc
00299abc ldr pc,[r4],#-0x4
00299ac0 cmp r4,r5
00299ac4 bcs 0x00299ab8
00299ac8 ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
00280144 UNCONDITIONAL_CALL
