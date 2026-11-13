002a3e98 cpy r12,sp
002a3e9c stmdb sp!,{r11,r12,lr,pc}
002a3ea0 sub r11,r12,#0x4
002a3ea4 bl 0x0029e310
002a3ea8 cmp r0,#0x0
002a3eac ldmiaeq sp,{r11,sp,pc}
002a3eb0 bl 0x002e83e0
002a3eb4 tst r0,#0x10000
002a3eb8 ldmiaeq sp,{r11,sp,pc}
002a3ebc ldr r0,[0x2a3edc]
002a3ec0 mov r1,#0x0
002a3ec4 bl 0x00339f50
002a3ec8 ldr r0,[0x2a3edc]
002a3ecc mov r1,#0x1
002a3ed0 cpy r2,r1
002a3ed4 bl 0x00339f64
002a3ed8 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002a3f44 UNCONDITIONAL_CALL
002a4020 UNCONDITIONAL_CALL
003146e8 DATA
00314778 DATA
