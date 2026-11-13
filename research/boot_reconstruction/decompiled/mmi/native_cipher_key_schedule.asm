00462fa8 cpy r12,sp
00462fac stmdb sp!,{r4,r5,r6,r11,r12,lr,pc}
00462fb0 sub r11,r12,#0x4
00462fb4 cpy r6,r0
00462fb8 mov r5,#0x0
00462fbc ldr r0,[0x463034]
00462fc0 mov r2,#0x40
00462fc4 bl 0x0046321c
00462fc8 ldr r0,[0x463034]
00462fcc cpy r1,r0
00462fd0 ldr r2,[0x463038]
00462fd4 mov r3,#0x38
00462fd8 bl 0x0046313c
00462fdc ldr r3,[0x46303c]
00462fe0 ldrb r4,[r3,r5]
00462fe4 ldr r3,[0x463040]
00462fe8 ldr r0,[r3,#0x0]
00462fec mov r1,#0x1c
00462ff0 cpy r2,r4
00462ff4 bl 0x004631c8
00462ff8 ldr r3,[0x463044]
00462ffc ldr r0,[r3,#0x0]
00463000 mov r1,#0x1c
00463004 cpy r2,r4
00463008 bl 0x004631c8
0046300c add r0,r5,r5, lsl #0x1
00463010 add r0,r6,r0, lsl #0x4
00463014 ldr r1,[0x463034]
00463018 ldr r2,[0x463048]
0046301c mov r3,#0x30
00463020 bl 0x0046313c
00463024 add r5,r5,#0x1
00463028 cmp r5,#0xf
0046302c ble 0x00462fdc
00463030 ldmia sp,{r4,r5,r6,r11,sp,pc}
REFERENCES TO ENTRY:
00462e2c UNCONDITIONAL_CALL
00462e44 UNCONDITIONAL_CALL
