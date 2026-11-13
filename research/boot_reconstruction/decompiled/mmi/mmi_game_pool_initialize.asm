002e3908 cpy r12,sp
002e390c stmdb sp!,{r11,r12,lr,pc}
002e3910 sub r11,r12,#0x4
002e3914 mov r0,#0x1
002e3918 mov r1,#0xff00
002e391c add r1,r1,#0xff
002e3920 bl 0x002e3898
002e3924 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
0056ad08 DATA
