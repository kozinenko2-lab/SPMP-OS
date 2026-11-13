002de24c cpy r12,sp
002de250 stmdb sp!,{r4,r11,r12,lr,pc}
002de254 sub r11,r12,#0x4
002de258 and r4,r0,#0xff
002de25c bl 0x002dd204
002de260 mov r0,r0, lsl #0x10
002de264 movs r0,r0, lsr #0x10
002de268 movne r3,r0, lsl #0x10
002de26c movne r0,r3, asr #0x10
002de270 ldmiane sp,{r4,r11,sp,pc}
002de274 cmp r4,#0x1
002de278 bne 0x002de2ac
002de27c ldr r4,[0x2de34c]
002de280 ldr r3,[0x2de350]
002de284 mov lr,pc
002de288 ldr pc,[r3,#0x0]
002de28c str r0,[r4,#0x4]
002de290 ldr r3,[0x2de354]
002de294 ldr r3,[r3,#0x0]
002de298 cpy r0,r4
002de29c ldrh r1,[r3,#0x8]
002de2a0 ldrh r2,[r3,#0xa]
002de2a4 bl 0x00312ce8
002de2a8 b 0x002de344
002de2ac cmp r4,#0x1
002de2b0 bls 0x002de2c4
002de2b4 ldr r3,[0x2de358]
002de2b8 ldrh r3,[r3,#0x0]
002de2bc cmp r3,r4
002de2c0 bhi 0x002de2d0
002de2c4 mov r0,#0x400
002de2c8 add r0,r0,#0x70000006
002de2cc ldmia sp,{r4,r11,sp,pc}
002de2d0 ldr r3,[0x2de354]
002de2d4 ldr r3,[r3,#0x0]
002de2d8 add r2,r4,r4, lsl #0x3
002de2dc add r3,r3,r2, lsl #0x2
002de2e0 ldrb r3,[r3,#-0x24]
002de2e4 cmp r3,#0x0
002de2e8 moveq r0,#0x400
002de2ec addeq r0,r0,#0x70000006
002de2f0 ldmiaeq sp,{r4,r11,sp,pc}
002de2f4 ldr r3,[0x2de354]
002de2f8 ldr r3,[r3,#0x0]
002de2fc add r2,r4,r4, lsl #0x3
002de300 add r3,r3,r2, lsl #0x2
002de304 ldr r3,[r3,#-0x20]
002de308 cmp r3,#0x0
002de30c movne r0,#0x400
002de310 addne r0,r0,#0x70000006
002de314 ldmiane sp,{r4,r11,sp,pc}
002de318 ldr r0,[0x2de34c]
002de31c ldr r3,[0x2de354]
002de320 ldr r3,[r3,#0x0]
002de324 add r2,r4,r4, lsl #0x3
002de328 add r3,r3,r2, lsl #0x2
002de32c sub r3,r3,#0x24
002de330 ldr r2,[r3,#0xc]
002de334 str r2,[r0,#0x4]
002de338 ldrh r1,[r3,#0x8]
002de33c ldrh r2,[r3,#0xa]
002de340 bl 0x00312ce8
002de344 mov r0,#0x0
002de348 ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
002d3420 DATA
