00461268 cpy r12,sp
0046126c stmdb sp!,{r4,r5,r6,r7,r11,r12,lr,pc}
00461270 sub r11,r12,#0x4
00461274 sub sp,sp,#0x4
00461278 cpy r5,r0
0046127c cpy r7,r0
00461280 mov r6,#0x0
00461284 sub r4,r11,#0x20
00461288 cpy r0,r4
0046128c cpy r1,r5
00461290 mov r2,#0x4
00461294 bl 0x00298c6c
00461298 add r5,r5,#0x4
0046129c cpy r0,r4
004612a0 ldr r1,[0x4614e0]
004612a4 bl 0x00346458
004612a8 cmp r0,r6
004612ac beq 0x00461394
004612b0 mov r1,#0x3
004612b4 mov r3,r1, lsl #0x10
004612b8 sub r0,r11,#0x1c
004612bc add r2,r0,r3, asr #0x10
004612c0 ldrb r3,[r5],#0x1
004612c4 strb r3,[r2,#-0x4]
004612c8 sub r3,r1,#0x1
004612cc mov r3,r3, lsl #0x10
004612d0 mov r1,r3, lsr #0x10
004612d4 movs r3,r1, lsl #0x10
004612d8 bpl 0x004612b8
004612dc ldr r3,[r11,#-0x20]
004612e0 add r6,r6,r3
004612e4 add r6,r6,#0x8
004612e8 add r5,r5,#0x2
004612ec mov r1,#0x1
004612f0 mov r3,r1, lsl #0x10
004612f4 sub lr,r11,#0x1c
004612f8 add r2,lr,r3, asr #0x10
004612fc ldrb r3,[r5],#0x1
00461300 strb r3,[r2,#-0x4]
00461304 sub r3,r1,#0x1
00461308 mov r3,r3, lsl #0x10
0046130c mov r1,r3, lsr #0x10
00461310 movs r3,r1, lsl #0x10
00461314 bpl 0x004612f4
00461318 add r5,r7,r6
0046131c mov r0,#0x0
00461320 ldrh r2,[r11,#-0x20]
00461324 ldrsh r3,[r11,#-0x20]
00461328 cmp r0,r3
0046132c bge 0x0046138c
00461330 mov r3,r2, lsl #0x10
00461334 mov r12,r3, asr #0x10
00461338 add r5,r5,#0x4
0046133c mov r1,#0x3
00461340 mov r3,r1, lsl #0x10
00461344 sub lr,r11,#0x1c
00461348 add r2,lr,r3, asr #0x10
0046134c ldrb r3,[r5],#0x1
00461350 strb r3,[r2,#-0x4]
00461354 sub r3,r1,#0x1
00461358 mov r3,r3, lsl #0x10
0046135c mov r1,r3, lsr #0x10
00461360 movs r3,r1, lsl #0x10
00461364 bpl 0x00461344
00461368 ldr r3,[r11,#-0x20]
0046136c add r5,r5,r3
00461370 add r6,r6,r3
00461374 add r6,r6,#0x8
00461378 add r3,r0,#0x1
0046137c mov r3,r3, lsl #0x10
00461380 mov r0,r3, lsr #0x10
00461384 cmp r12,r3, asr #0x10
00461388 bgt 0x00461338
0046138c cpy r0,r6
00461390 ldmib sp,{r4,r5,r6,r7,r11,sp,pc}
00461394 sub r0,r11,#0x20
00461398 ldr r1,[0x4614e4]
0046139c bl 0x00346458
004613a0 cmp r0,#0x0
004613a4 beq 0x004613cc
004613a8 ldrb r6,[r5,#0x0]
004613ac ldrb r3,[r5,#0x1]
004613b0 orr r6,r6,r3, lsl #0x8
004613b4 ldrb r3,[r5,#0x2]
004613b8 orr r6,r6,r3, lsl #0x10
004613bc ldrb r3,[r5,#0x3]
004613c0 orr r6,r6,r3, lsl #0x18
004613c4 add r0,r6,#0x8
004613c8 ldmib sp,{r4,r5,r6,r7,r11,sp,pc}
004613cc sub r0,r11,#0x20
004613d0 ldr r1,[0x4614e8]
004613d4 bl 0x00346458
004613d8 cmp r0,#0x0
004613dc moveq r0,#0x0
004613e0 ldmibeq sp,{r4,r5,r6,r7,r11,sp,pc}
004613e4 mov r1,#0x3
004613e8 mov r3,r1, lsl #0x10
004613ec sub r0,r11,#0x1c
004613f0 add r2,r0,r3, asr #0x10
004613f4 ldrb r3,[r5],#0x1
004613f8 strb r3,[r2,#-0x4]
004613fc sub r3,r1,#0x1
00461400 mov r3,r3, lsl #0x10
00461404 mov r1,r3, lsr #0x10
00461408 movs r3,r1, lsl #0x10
0046140c bpl 0x004613ec
00461410 ldr r3,[r11,#-0x20]
00461414 add r6,r6,r3
00461418 add r6,r6,#0x8
0046141c add r5,r5,r3
00461420 add r5,r5,#0x4
00461424 mov r1,#0x3
00461428 mov r3,r1, lsl #0x10
0046142c sub lr,r11,#0x1c
00461430 add r2,lr,r3, asr #0x10
00461434 ldrb r3,[r5],#0x1
00461438 strb r3,[r2,#-0x4]
0046143c sub r3,r1,#0x1
00461440 mov r3,r3, lsl #0x10
00461444 mov r1,r3, lsr #0x10
00461448 movs r3,r1, lsl #0x10
0046144c bpl 0x0046142c
00461450 ldr r3,[r11,#-0x20]
00461454 add r6,r6,r3
00461458 add r6,r6,#0x8
0046145c add r5,r5,r3
00461460 add r5,r5,#0x4
00461464 mov r1,#0x3
00461468 mov r3,r1, lsl #0x10
0046146c sub r0,r11,#0x1c
00461470 add r2,r0,r3, asr #0x10
00461474 ldrb r3,[r5],#0x1
00461478 strb r3,[r2,#-0x4]
0046147c sub r3,r1,#0x1
00461480 mov r3,r3, lsl #0x10
00461484 mov r1,r3, lsr #0x10
00461488 movs r3,r1, lsl #0x10
0046148c bpl 0x0046146c
00461490 ldr r3,[r11,#-0x20]
00461494 add r6,r6,r3
00461498 add r6,r6,#0x8
0046149c add r5,r5,r3
004614a0 add r5,r5,#0x4
004614a4 mov r1,#0x3
004614a8 mov r3,r1, lsl #0x10
004614ac sub lr,r11,#0x1c
004614b0 add r2,lr,r3, asr #0x10
004614b4 ldrb r3,[r5],#0x1
004614b8 strb r3,[r2,#-0x4]
004614bc sub r3,r1,#0x1
004614c0 mov r3,r3, lsl #0x10
004614c4 mov r1,r3, lsr #0x10
004614c8 movs r3,r1, lsl #0x10
004614cc bpl 0x004614ac
004614d0 ldr r3,[r11,#-0x20]
004614d4 add r6,r6,r3
004614d8 add r0,r6,#0x8
004614dc ldmib sp,{r4,r5,r6,r7,r11,sp,pc}
REFERENCES TO ENTRY:
00462060 UNCONDITIONAL_CALL
