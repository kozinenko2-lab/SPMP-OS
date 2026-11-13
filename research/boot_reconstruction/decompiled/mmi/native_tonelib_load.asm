00462c94 cpy r12,sp
00462c98 stmdb sp!,{r4,r11,r12,lr,pc}
00462c9c sub r11,r12,#0x4
00462ca0 sub sp,sp,#0x4
00462ca4 bl 0x00461e14
00462ca8 add r4,r0,#0x100000
00462cac cpy r0,r4
00462cb0 ldr r1,[0x462ccc]
00462cb4 mov r2,#0x32000
00462cb8 sub r3,r11,#0x14
00462cbc bl 0x0045ffcc
00462cc0 cpy r0,r4
00462cc4 bl 0x00464f78
00462cc8 ldmib sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
00461ef8 UNCONDITIONAL_CALL
