0003e48c stmdb sp!,{r4,r5,r6,r7,lr}
0003e490 cpy r5,r0
0003e494 cpy r3,r1
0003e498 cmp r1,#0x0
0003e49c cmpne r0,#0x0
0003e4a0 beq 0x0003e54c
0003e4a4 ands r1,r2,#0x3
0003e4a8 bne 0x0003e54c
0003e4ac cpy r6,r3
0003e4b0 mov lr,r2, lsr #0x4
0003e4b4 cpy r4,r1
0003e4b8 cpy r7,r1
0003e4bc tst r7,#0x1
0003e4c0 bne 0x0003e4d8
0003e4c4 cmp lr,#0x0
0003e4c8 cpyne r12,r6
0003e4cc movne r0,#0x0
0003e4d0 bne 0x0003e514
0003e4d4 b 0x0003e530
0003e4d8 tst r7,#0x2
0003e4dc movne r3,r2, lsl #0x1c
0003e4e0 addne lr,lr,r3, lsr #0x1e
0003e4e4 cmp lr,#0x0
0003e4e8 beq 0x0003e530
0003e4ec cpy r12,r6
0003e4f0 mov r0,#0x0
0003e4f4 ldr r3,[r12],#0x4
0003e4f8 add r4,r4,r3
0003e4fc add r0,r0,#0x1
0003e500 cmp r0,lr
0003e504 bne 0x0003e4f4
0003e508 mov r3,lr, lsl #0x2
0003e50c add r6,r6,r3
0003e510 b 0x0003e530
0003e514 ldr r3,[r12],#0x4
0003e518 add r1,r1,r3
0003e51c add r0,r0,#0x1
0003e520 cmp r0,lr
0003e524 bne 0x0003e514
0003e528 mov r3,lr, lsl #0x2
0003e52c add r6,r6,r3
0003e530 add r7,r7,#0x1
0003e534 cmp r7,#0x4
0003e538 bne 0x0003e4bc
0003e53c str r1,[r5,#0x8]
0003e540 str r4,[r5,#0xc]
0003e544 mov r0,#0x1
0003e548 ldmia sp!,{r4,r5,r6,r7,pc}
0003e54c mvn r0,#0x0
0003e550 ldmia sp!,{r4,r5,r6,r7,pc}
REFERENCES TO ENTRY:
0003e810 UNCONDITIONAL_CALL
