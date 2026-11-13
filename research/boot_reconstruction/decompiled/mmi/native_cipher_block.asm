00462e68 cpy r12,sp
00462e6c stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr,pc}
00462e70 sub r11,r12,#0x4
00462e74 cpy r9,r0
00462e78 cpy r10,r2
00462e7c and r4,r3,#0xff
00462e80 mov r5,#0x0
00462e84 ldr r0,[0x462f90]
00462e88 mov r2,#0x40
00462e8c bl 0x0046321c
00462e90 ldr r0,[0x462f90]
00462e94 cpy r1,r0
00462e98 ldr r2,[0x462f94]
00462e9c mov r3,#0x40
00462ea0 bl 0x0046313c
00462ea4 cmp r4,r5
00462ea8 movne r5,#0xf
00462eac ldrne r8,[0x462f98]
00462eb0 ldrne r7,[0x462f9c]
00462eb4 movne r6,#0x20
00462eb8 bne 0x00462f1c
00462ebc ldr r8,[0x462f98]
00462ec0 ldr r7,[0x462fa0]
00462ec4 mov r6,#0x20
00462ec8 cpy r0,r8
00462ecc ldr r1,[r7,#0x0]
00462ed0 cpy r2,r6
00462ed4 bl 0x00298c6c
00462ed8 add r1,r5,r5, lsl #0x1
00462edc ldr r0,[r7,#0x0]
00462ee0 add r1,r10,r1, lsl #0x4
00462ee4 bl 0x0046304c
00462ee8 ldr r4,[0x462f9c]
00462eec ldr r0,[r7,#0x0]
00462ef0 ldr r1,[r4,#0x0]
00462ef4 cpy r2,r6
00462ef8 bl 0x00463194
00462efc ldr r0,[r4,#0x0]
00462f00 cpy r1,r8
00462f04 cpy r2,r6
00462f08 bl 0x00298c6c
00462f0c add r5,r5,#0x1
00462f10 cmp r5,#0xf
00462f14 ble 0x00462ec8
00462f18 b 0x00462f68
00462f1c cpy r0,r8
00462f20 ldr r1,[r7,#0x0]
00462f24 cpy r2,r6
00462f28 bl 0x00298c6c
00462f2c add r1,r5,r5, lsl #0x1
00462f30 ldr r0,[r7,#0x0]
00462f34 add r1,r10,r1, lsl #0x4
00462f38 bl 0x0046304c
00462f3c ldr r4,[0x462fa0]
00462f40 ldr r0,[r7,#0x0]
00462f44 ldr r1,[r4,#0x0]
00462f48 cpy r2,r6
00462f4c bl 0x00463194
00462f50 ldr r0,[r4,#0x0]
00462f54 cpy r1,r8
00462f58 cpy r2,r6
00462f5c bl 0x00298c6c
00462f60 subs r5,r5,#0x1
00462f64 bpl 0x00462f1c
00462f68 ldr r0,[0x462f90]
00462f6c cpy r1,r0
00462f70 ldr r2,[0x462fa4]
00462f74 mov r3,#0x40
00462f78 bl 0x0046313c
00462f7c cpy r0,r9
00462f80 ldr r1,[0x462f90]
00462f84 mov r2,#0x40
00462f88 bl 0x00463254
00462f8c ldmia sp,{r4,r5,r6,r7,r8,r9,r10,r11,sp,pc}
REFERENCES TO ENTRY:
00462d50 UNCONDITIONAL_CALL
00462d90 UNCONDITIONAL_CALL
00462da8 UNCONDITIONAL_CALL
00462dbc UNCONDITIONAL_CALL
