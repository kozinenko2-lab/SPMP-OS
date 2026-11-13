00292198 stmdb sp!,{r4,lr}
0029219c mov r1,#0x0
002921a0 ldr r12,[0x29222c]
002921a4 mov r4,#0x1
002921a8 cpy r0,r1
002921ac ldr lr,[0x292230]
002921b0 add r3,r1,r1, lsl #0x2
002921b4 mov r3,r3, lsl #0x2
002921b8 add r2,r3,r12
002921bc str r4,[r2,#0x4]
002921c0 str r0,[r3,r12]
002921c4 str r0,[r2,#0x8]
002921c8 add r3,r3,lr
002921cc str r3,[r2,#0x10]
002921d0 add r1,r1,#0x1
002921d4 cmp r1,#0x2
002921d8 ble 0x002921b0
002921dc ldr r2,[0x29222c]
002921e0 add r3,r1,r1, lsl #0x2
002921e4 add r3,r2,r3, lsl #0x2
002921e8 str r2,[r3,#-0x4]
002921ec ldr r3,[0x292234]
002921f0 str r2,[r3,#0x0]
002921f4 ldr r3,[0x292238]
002921f8 str r2,[r3,#0x0]
002921fc ldr r3,[0x29223c]
00292200 str r2,[r3,#0x0]
00292204 ldr r3,[0x292240]
00292208 str r2,[r3,#0x0]
0029220c mov r0,#0x0
00292210 ldr r3,[0x292244]
00292214 str r0,[r3,#0x0]
00292218 ldr r2,[0x292248]
0029221c ldr r3,[r2,#0x0]
00292220 add r3,r3,#0x1
00292224 str r3,[r2,#0x0]
00292228 ldmia sp!,{r4,pc}
REFERENCES TO ENTRY:
002c1c08 UNCONDITIONAL_CALL
0029292c UNCONDITIONAL_CALL
002c55ac UNCONDITIONAL_CALL
