003be6a0 cpy r12,sp
003be6a4 stmdb sp!,{r11,r12,lr,pc}
003be6a8 sub r11,r12,#0x4
003be6ac bl 0x003be70c
003be6b0 cmp r0,#0x0
003be6b4 movge r0,#0x1
003be6b8 mvnlt r0,#0x0
003be6bc ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
003be64c UNCONDITIONAL_CALL
