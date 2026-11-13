002deab8 cpy r12,sp
002deabc stmdb sp!,{r11,r12,lr,pc}
002deac0 sub r11,r12,#0x4
002deac4 cmp r0,#0x0
002deac8 beq 0x002deaf0
002deacc cmp r0,#0x1
002dead0 bne 0x002deb08
002dead4 ldr r3,[0x2deb10]
002dead8 mov lr,pc
002deadc ldr pc,[r3,#0x0]
002deae0 ldr r3,[0x2deb14]
002deae4 str r0,[r3,#0x4]
002deae8 bl 0x002dd204
002deaec b 0x002deb08
002deaf0 ldr r3,[0x2deb18]
002deaf4 mov lr,pc
002deaf8 ldr pc,[r3,#0x0]
002deafc ldr r3,[0x2deb14]
002deb00 str r0,[r3,#0x4]
002deb04 bl 0x002dd204
002deb08 mov r0,#0x0
002deb0c ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002de1dc UNCONDITIONAL_CALL
002de1e8 UNCONDITIONAL_CALL
002d3440 DATA
