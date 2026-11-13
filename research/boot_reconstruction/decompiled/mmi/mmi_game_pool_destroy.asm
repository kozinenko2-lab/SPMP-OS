002e3928 cpy r12,sp
002e392c stmdb sp!,{r11,r12,lr,pc}
002e3930 sub r11,r12,#0x4
002e3934 mov r0,#0x0
002e3938 mov r1,#0xff00
002e393c add r1,r1,#0xff
002e3940 bl 0x002e3898
002e3944 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
0056ad5c DATA
