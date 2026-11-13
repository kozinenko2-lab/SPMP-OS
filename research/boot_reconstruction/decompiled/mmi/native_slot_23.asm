002de6d8 cpy r12,sp
002de6dc stmdb sp!,{r11,r12,lr,pc}
002de6e0 sub r11,r12,#0x4
002de6e4 ldrh r1,[r0,#0x0]
002de6e8 ldrh r3,[r0,#0x4]
002de6ec add r3,r1,r3
002de6f0 mov r3,r3, lsl #0x10
002de6f4 mov r12,r3, lsr #0x10
002de6f8 ldrh r3,[r0,#0x2]
002de6fc ldrh r2,[r0,#0x6]
002de700 add r3,r3,r2
002de704 mov r3,r3, lsl #0x10
002de708 mov lr,r3, lsr #0x10
002de70c add r1,r1,#0x400
002de710 mov r1,r1, lsl #0x10
002de714 mov r3,#0x7f0
002de718 add r3,r3,#0xf
002de71c cmp r3,r1, lsr #0x10
002de720 bcc 0x002de788
002de724 ldrsh r2,[r0,#0x2]
002de728 cmn r2,#0x400
002de72c blt 0x002de788
002de730 mov r1,#0x3fc
002de734 add r1,r1,#0x3
002de738 mov r3,r12, lsl #0x10
002de73c mov r12,r3, asr #0x10
002de740 cmn r12,#0x400
002de744 movge r3,#0x0
002de748 movlt r3,#0x1
002de74c cmp r2,r1
002de750 orrgt r3,r3,#0x1
002de754 cmp r3,#0x0
002de758 bne 0x002de788
002de75c mov r3,lr, lsl #0x10
002de760 mov r2,r3, asr #0x10
002de764 cmn r2,#0x400
002de768 movge r3,#0x0
002de76c movlt r3,#0x1
002de770 cmp r12,r1
002de774 orrgt r3,r3,#0x1
002de778 cmp r3,#0x0
002de77c bne 0x002de788
002de780 cmp r2,r1
002de784 ble 0x002de794
002de788 mov r0,#0x400
002de78c add r0,r0,#0x70000001
002de790 ldmia sp,{r11,sp,pc}
002de794 ldr r1,[0x2de7c8]
002de798 ldrh r3,[r0,#0x0]
002de79c strh r3,[r1,#0x0]
002de7a0 ldrh r3,[r0,#0x2]
002de7a4 strh r3,[r1,#0x2]
002de7a8 ldrh r3,[r0,#0x4]
002de7ac strh r3,[r1,#0x4]
002de7b0 ldrh r0,[r0,#0x6]
002de7b4 strh r0,[r1,#0x6]
002de7b8 ldr r0,[0x2de7cc]
002de7bc bl 0x00312e94
002de7c0 mov r0,#0x0
002de7c4 ldmia sp,{r11,sp,pc}
REFERENCES TO ENTRY:
002d33a8 DATA
