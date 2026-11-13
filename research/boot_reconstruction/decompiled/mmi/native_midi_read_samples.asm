00464da8 stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
00464dac ldr r5,[0x464d8c]
00464db0 sub sp,sp,#0x8
00464db4 mov r8,#0x0
00464db8 ldr r6,[0x464d64]
00464dbc cpy r10,r1
00464dc0 cpy r9,r0
00464dc4 sub r7,r5,#0x200
00464dc8 str r8,[sp,#0x4]
00464dcc b 0x00464e70
00464dd0 ldrsh r0,[r6,#0x4]
00464dd4 cmp r0,#0x0
00464dd8 subne r0,r0,#0x1
00464ddc strhne r0,[r6,#0x4]
00464de0 bne 0x00464e60
00464de4 bl 0x00465874
00464de8 ldrsh r0,[r6,#0x2]
00464dec mov r4,#0x0
00464df0 cmp r0,#0x0
00464df4 bls 0x00464e48
00464df8 add r0,r7,r4, lsl #0x1
00464dfc ldrh r0,[r0,#0x0]
00464e00 cmp r0,#0x0
00464e04 beq 0x00464e38
00464e08 ldr r0,[r5,r4,lsl #0x2]
00464e0c cmp r0,#0x0
00464e10 bne 0x00464e2c
00464e14 ldr r0,[r6,#0x18]
00464e18 bl 0x00465210
00464e1c mov r0,r4, lsl #0x10
00464e20 mov r0,r0, lsr #0x10
00464e24 bl 0x004647e8
00464e28 str r8,[r6,#0x18]
00464e2c ldr r0,[r5,r4,lsl #0x2]
00464e30 sub r0,r0,#0x1
00464e34 str r0,[r5,r4,lsl #0x2]
00464e38 ldrsh r0,[r6,#0x2]
00464e3c add r4,r4,#0x1
00464e40 cmp r4,r0
00464e44 bcc 0x00464df8
00464e48 bl 0x004658ec
00464e4c ldr r0,[r6,#0xc]
00464e50 strh r0,[r6,#0x4]
00464e54 ldr r0,[r6,#0x18]
00464e58 add r0,r0,#0x1
00464e5c str r0,[r6,#0x18]
00464e60 ldr r0,[sp,#0x4]
00464e64 add r1,sp,#0x4
00464e68 add r0,r9,r0
00464e6c bl 0x00465220
00464e70 ldr r0,[sp,#0x4]
00464e74 cmp r0,r10
00464e78 bge 0x00464e88
00464e7c ldrsh r0,[r6,#0x0]
00464e80 cmp r0,#0x0
00464e84 bgt 0x00464dd0
00464e88 ldr r0,[sp,#0x4]
00464e8c add sp,sp,#0x8
00464e90 ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
REFERENCES TO ENTRY:
004621e8 UNCONDITIONAL_CALL
