000407c0 stmdb sp!,{r4,r5,r6,r7,r8,r10,lr}
000407c4 cpy r10,r1
000407c8 cpy r8,r0
000407cc ldr r2,[r0,#0x2c]
000407d0 ldr r3,[r0,#0x28]
000407d4 ldr r7,[r2,r3,lsl #0x2]
000407d8 ldr r3,[r0,#0x24]
000407dc ldr r6,[r2,r3,lsl #0x2]
000407e0 mov r2,r3, lsl #0x3
000407e4 mov r3,r3, lsl #0x5
000407e8 add r2,r2,r3
000407ec ldr r3,[r0,#0x20]
000407f0 add r2,r2,r3
000407f4 ldr r0,[r2,#0x14]
000407f8 ldr r1,[r2,#0x24]
000407fc bl 0x0004bc08
00040800 subs r5,r0,#0x0
00040804 ble 0x00040840
00040808 mov r4,#0x0
0004080c ldr r0,[r6,r4,lsl #0x4]
00040810 add r0,r7,r0
00040814 cpy r1,r10
00040818 bl 0x0004caf4
0004081c cmp r0,#0x0
00040820 bne 0x00040834
00040824 cpy r0,r8
00040828 cpy r1,r4
0004082c bl 0x00040f9c
00040830 ldmia sp!,{r4,r5,r6,r7,r8,r10,pc}
00040834 add r4,r4,#0x1
00040838 cmp r5,r4
0004083c bne 0x0004080c
00040840 ldr r2,[0x40854]
00040844 ldr r3,[0x40858]
00040848 str r2,[r3,#0x0]
0004084c mov r0,#0x0
00040850 ldmia sp!,{r4,r5,r6,r7,r8,r10,pc}
REFERENCES TO ENTRY:
00036a74 UNCONDITIONAL_CALL
00040e68 UNCONDITIONAL_CALL
