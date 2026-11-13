002def10 cpy r12,sp
002def14 stmdb sp!,{r11,r12,lr,pc}
002def18 sub r11,r12,#0x4
002def1c ldr r0,[0x2def2c]
002def20 bl 0x00298b64
002def24 mov r0,#0x0
002def28 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002d3480 DATA
