002e37c0 cpy r12,sp
002e37c4 stmdb sp!,{r4,r11,r12,lr,pc}
002e37c8 sub r11,r12,#0x4
002e37cc sub sp,sp,#0x2c
002e37d0 cpy r4,r0
002e37d4 mov r3,#0x0
002e37d8 cpy r2,r0
002e37dc str r3,[r2],#0x4
002e37e0 str r3,[r0,#0x4]
002e37e4 add r2,r2,#0x4
002e37e8 str r3,[r2],#0x4
002e37ec str r3,[r2],#0x4
002e37f0 str r3,[r2],#0x4
002e37f4 str r3,[r2],#0x4
002e37f8 str r3,[r2],#0x4
002e37fc str r3,[r2],#0x4
002e3800 str r3,[r2],#0x4
002e3804 str r3,[r2],#0x4
002e3808 str r3,[r2,#0x0]
002e380c sub r3,r3,#0x1
002e3810 str r3,[r11,#-0x3c]
002e3814 str r3,[r11,#-0x38]
002e3818 str r3,[r11,#-0x34]
002e381c str r3,[r11,#-0x30]
002e3820 str r3,[r11,#-0x2c]
002e3824 str r3,[r11,#-0x28]
002e3828 str r3,[r11,#-0x24]
002e382c strb r3,[r11,#-0x20]
002e3830 str r3,[r11,#-0x1c]
002e3834 str r3,[r11,#-0x18]
002e3838 strb r3,[r11,#-0x14]
002e383c ldr r0,[0x2e3894]
002e3840 mov r1,#0x5e
002e3844 sub r2,r11,#0x3c
002e3848 bl 0x0032e5e4
002e384c ldr r3,[r11,#-0x38]
002e3850 cmp r3,#0x0
002e3854 strgt r3,[r4,#0x0]
002e3858 ldr r3,[r11,#-0x34]
002e385c cmp r3,#0x0
002e3860 strgt r3,[r4,#0x4]
002e3864 ldr r3,[r11,#-0x30]
002e3868 cmp r3,#0x0
002e386c strgt r3,[r4,#0x1c]
002e3870 ldr r3,[r11,#-0x2c]
002e3874 cmp r3,#0x0
002e3878 strgt r3,[r4,#0x20]
002e387c ldr r3,[r11,#-0x24]
002e3880 cmp r3,#0x0
002e3884 strgt r3,[r4,#0x28]
002e3888 cpy r0,r4
002e388c sub sp,r11,#0x10
002e3890 ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
003b966c UNCONDITIONAL_CALL
003d7934 UNCONDITIONAL_CALL
003d7f58 UNCONDITIONAL_CALL
002cba10 UNCONDITIONAL_CALL
002d7904 UNCONDITIONAL_CALL
00337508 UNCONDITIONAL_CALL
00460d48 UNCONDITIONAL_CALL
00460db0 UNCONDITIONAL_CALL
