002a4aa0 cpy r12,sp
002a4aa4 stmdb sp!,{r4,r11,r12,lr,pc}
002a4aa8 sub r11,r12,#0x4
002a4aac bl 0x0029e310
002a4ab0 cpy r4,r0
002a4ab4 ldr r3,[0x2a4b50]
002a4ab8 ldr r3,[r3,#0x0]
002a4abc cmp r3,#0x0
002a4ac0 ldmiaeq sp,{r4,r11,sp,pc}
002a4ac4 cmp r0,#0x0
002a4ac8 bne 0x002a4b44
002a4acc ldr r4,[0x2a4b54]
002a4ad0 ldr r3,[r4,#0x0]
002a4ad4 cmp r3,#0x0
002a4ad8 beq 0x002a4b44
002a4adc mov r0,#0x1
002a4ae0 bl 0x002e7cf0
002a4ae4 mov r0,#0x1
002a4ae8 bl 0x002e77c0
002a4aec ldr r3,[r4,#0x0]
002a4af0 subs r4,r3,#0x1
002a4af4 beq 0x002a4b04
002a4af8 bl 0x0029e244
002a4afc subs r4,r4,#0x1
002a4b00 bne 0x002a4af8
002a4b04 mov r2,#0x0
002a4b08 ldr r3,[0x2a4b54]
002a4b0c str r2,[r3,#0x0]
002a4b10 ldr r3,[0x2a4b58]
002a4b14 ldr r3,[r3,#0x0]
002a4b18 tst r3,#0x1
002a4b1c movne r0,#0x1
002a4b20 blne 0x002e788c
002a4b24 ldr r3,[0x2a4b58]
002a4b28 ldr r3,[r3,#0x0]
002a4b2c tst r3,#0x2
002a4b30 movne r0,#0x1
002a4b34 blne 0x002e7860
002a4b38 mov r2,#0x0
002a4b3c ldr r3,[0x2a4b58]
002a4b40 str r2,[r3,#0x0]
002a4b44 mov r0,#0x17
002a4b48 bl 0x00339cd8
002a4b4c ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
002d5aec COMPUTED_CALL
002d6194 COMPUTED_CALL
002d8e2c COMPUTED_CALL
002d8fbc COMPUTED_CALL
00314700 DATA
00314790 DATA
