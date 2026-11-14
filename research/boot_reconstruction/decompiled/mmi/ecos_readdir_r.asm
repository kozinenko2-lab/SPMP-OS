002a6b8c cpy r12,sp
002a6b90 stmdb sp!,{r4,r5,r11,r12,lr,pc}
002a6b94 sub r11,r12,#0x4
002a6b98 cpy r5,r1
002a6b9c cpy r4,r2
002a6ba0 mov r3,#0x0
002a6ba4 str r3,[r2,#0x0]
002a6ba8 cmp r0,#0x0
002a6bac moveq r0,#0x9
002a6bb0 ldmiaeq sp,{r4,r5,r11,sp,pc}
002a6bb4 mov r2,#0x104
002a6bb8 bl 0x002eb5e0
002a6bbc cmp r0,#0x0
002a6bc0 bge 0x002a6bd0
002a6bc4 bl 0x00299a14
002a6bc8 ldr r0,[r0,#0x0]
002a6bcc ldmia sp,{r4,r5,r11,sp,pc}
002a6bd0 cmp r0,#0x0
002a6bd4 strgt r5,[r4,#0x0]
002a6bd8 mov r0,#0x0
002a6bdc ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002a6b68 UNCONDITIONAL_CALL
002a67f8 UNCONDITIONAL_CALL
