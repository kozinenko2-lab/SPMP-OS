002e77c0 cpy r12,sp
002e77c4 stmdb sp!,{r4,r11,r12,lr,pc}
002e77c8 sub r11,r12,#0x4
002e77cc cmp r0,#0x0
002e77d0 beq 0x002e7820
002e77d4 ldr r3,[0x2e7858]
002e77d8 ldr r3,[r3,#0x0]
002e77dc cmp r3,#0x0
002e77e0 bne 0x002e7810
002e77e4 ldr r4,[0x2e785c]
002e77e8 ldrh r0,[r4,#0xa]
002e77ec ldrh r1,[r4,#0x8]
002e77f0 bl 0x002e80ec
002e77f4 mov r1,#0x93000000
002e77f8 ldr r2,[r1,#0x0]
002e77fc ldr r3,[r4,#0x0]
002e7800 and r3,r3,#0x300000
002e7804 orr r2,r2,r3, lsr #0x14
002e7808 orr r2,r2,#0x10000
002e780c str r2,[r1,#0x0]
002e7810 mov r2,#0x1
002e7814 ldr r3,[0x2e7858]
002e7818 str r2,[r3,#0x0]
002e781c b 0x002e7844
002e7820 ldr r3,[0x2e7858]
002e7824 ldr r3,[r3,#0x0]
002e7828 cmp r3,#0x0
002e782c movne r2,#0x0
002e7830 movne r3,#0x93000000
002e7834 strne r2,[r3,#0x0]
002e7838 mov r2,#0x0
002e783c ldr r3,[0x2e7858]
002e7840 str r2,[r3,#0x0]
002e7844 mov r3,#0x93000000
002e7848 add r3,r3,#0x78
002e784c mov r2,#0x1
002e7850 str r2,[r3,#0x0]
002e7854 ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
002e8374 UNCONDITIONAL_CALL
002e7db4 CONDITIONAL_CALL
002a4ae8 UNCONDITIONAL_CALL
002a4884 UNCONDITIONAL_CALL
