00294dfc cpy r12,sp
00294e00 stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr,pc}
00294e04 sub r11,r12,#0x4
00294e08 mov r7,#0x0
00294e0c cpy r5,r7
00294e10 cpy r8,r7
00294e14 cpy r6,r7
00294e18 cpy r10,r7
00294e1c ldr r3,[0x294f14]
00294e20 ldr r4,[r3,#0x0]
00294e24 ldr r3,[0x294f18]
00294e28 ldr r0,[r3,#0x0]
00294e2c ldr r3,[0x294f1c]
00294e30 ldr r1,[r3,#0x0]
00294e34 subs r2,r0,r1
00294e38 ldrmi r3,[0x294f20]
00294e3c ldrmi r3,[r3,#0x0]
00294e40 rsbmi r2,r1,r3
00294e44 addmi r2,r2,r0
00294e48 cmp r2,#0x18000
00294e4c cpylt r7,r2
00294e50 movge r7,#0x18000
00294e54 ldr r3,[0x294f24]
00294e58 smull r1,r2,r3,r7
00294e5c mov r3,r7, asr #0x1f
00294e60 rsb r9,r3,r2, asr #0x5
00294e64 ldr r0,[0x294f28]
00294e68 cpy r1,r9
00294e6c cpy r2,r7
00294e70 bl 0x00298b64
00294e74 ldr r2,[0x294f24]
00294e78 ldrsh r3,[r4,#0x0]
00294e7c cmp r3,#0x0
00294e80 bge 0x00294eb0
00294e84 ldrh r3,[r4,#0x0]
00294e88 rsb r3,r3,#0x0
00294e8c mov r3,r3, lsl #0x10
00294e90 mov r3,r3, lsr #0x10
00294e94 mul r3,r10,r3
00294e98 umull r1,r3,r2,r3
00294e9c mov r3,r3, lsl #0xb
00294ea0 mvn r3,r3, lsr #0x10
00294ea4 add r3,r3,#0x1
00294ea8 strh r3,[r4,#0x0]
00294eac b 0x00294ec4
00294eb0 ldrh r3,[r4,#0x0]
00294eb4 mul r3,r10,r3
00294eb8 umull r1,r3,r2,r3
00294ebc mov r3,r3, lsr #0x5
00294ec0 strh r3,[r4,#0x0]
00294ec4 add r6,r6,#0x2
00294ec8 add r8,r8,#0x2
00294ecc add r5,r5,#0x2
00294ed0 ldr r3,[0x294f20]
00294ed4 ldr r3,[r3,#0x0]
00294ed8 cmp r5,r3
00294edc movgt r5,#0x0
00294ee0 ldr r3,[0x294f14]
00294ee4 ldr r3,[r3,#0x0]
00294ee8 add r4,r5,r3
00294eec cmp r6,r9
00294ef0 addgt r10,r10,#0x1
00294ef4 movgt r6,#0x0
00294ef8 cmp r8,r7
00294efc blt 0x00294e78
00294f00 ldr r0,[0x294f2c]
00294f04 cpy r1,r10
00294f08 bl 0x00298b64
00294f0c mov r0,#0x0
00294f10 ldmia sp,{r4,r5,r6,r7,r8,r9,r10,r11,sp,pc}
REFERENCES TO ENTRY:
002c2038 CONDITIONAL_CALL
