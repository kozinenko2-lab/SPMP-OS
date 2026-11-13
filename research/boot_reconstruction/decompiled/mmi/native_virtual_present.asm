002def60 cpy r12,sp
002def64 stmdb sp!,{r4,r5,r6,r11,r12,lr,pc}
002def68 sub r11,r12,#0x4
002def6c sub sp,sp,#0x18
002def70 ldr r5,[0x2deff4]
002def74 ldr r1,[r5,#0x0]
002def78 ldr r2,[0x2deff8]
002def7c ldr r3,[r2,#0x0]
002def80 str r3,[r5,#0x0]
002def84 str r1,[r2,#0x0]
002def88 ldr r6,[0x2deffc]
002def8c mov lr,pc
002def90 ldr pc,[r6,#0x10]
002def94 bl 0x002e5c60
002def98 mov lr,pc
002def9c ldr pc,[r6,#0x8]
002defa0 cpy r4,r0
002defa4 mov lr,pc
002defa8 ldr pc,[r6,#0xc]
002defac str r4,[sp,#0x0]
002defb0 str r0,[sp,#0x4]
002defb4 mov r3,#0x0
002defb8 str r3,[sp,#0x8]
002defbc ldr r2,[r5,#0x0]
002defc0 str r2,[sp,#0xc]
002defc4 str r3,[sp,#0x10]
002defc8 str r3,[sp,#0x14]
002defcc cpy r0,r3
002defd0 mov r1,#0x140
002defd4 mov r2,#0xf0
002defd8 bl 0x002e6004
002defdc mov r0,#0x1
002defe0 bl 0x002e65e4
002defe4 mov lr,pc
002defe8 ldr pc,[r6,#0x18]
002defec sub sp,r11,#0x18
002deff0 ldmia sp,{r4,r5,r6,r11,sp,pc}
REFERENCES TO ENTRY:
002df0a4 DATA
002df09c PARAM
002df104 DATA
002df0fc PARAM
