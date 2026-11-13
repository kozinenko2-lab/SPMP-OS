002c567c cpy r12,sp
002c5680 stmdb sp!,{r4,r5,r6,r11,r12,lr,pc}
002c5684 sub r11,r12,#0x4
002c5688 cpy r6,r0
002c568c cpy r4,r1
002c5690 ldr r3,[0x2c583c]
002c5694 ldr r3,[r3,#0x0]
002c5698 cmp r3,#0x0
002c569c bne 0x002c578c
002c56a0 ldr r3,[0x2c5840]
002c56a4 ldr r3,[r3,#0x0]
002c56a8 ldr r3,[r3,#0x0]
002c56ac tst r3,#0x1
002c56b0 ldrne r0,[0x2c5844]
002c56b4 cpyne r1,r6
002c56b8 blne 0x00298b64
002c56bc ldr r3,[0x2c5848]
002c56c0 ldr r3,[r3,#0x0]
002c56c4 cmp r3,#0x1
002c56c8 bne 0x002c56dc
002c56cc mov r0,r4, lsl #0x1
002c56d0 mov r1,#0x0
002c56d4 bl 0x00294ab0
002c56d8 b 0x002c56e8
002c56dc cpy r0,r4
002c56e0 mov r1,#0x0
002c56e4 bl 0x00294ab0
002c56e8 cmp r0,#0x0
002c56ec ble 0x002c5760
002c56f0 cpy lr,r6
002c56f4 mov r12,#0x0
002c56f8 ldr r3,[0x2c5848]
002c56fc ldr r3,[r3,#0x0]
002c5700 cmp r3,#0x1
002c5704 bne 0x002c5744
002c5708 mov r3,r12, lsl #0x2
002c570c add r1,r3,r0
002c5710 mov r2,r12, lsl #0x1
002c5714 ldrh r5,[r2,lr]
002c5718 strh r5,[r3,r0]
002c571c ldrh r2,[r2,lr]
002c5720 strh r2,[r1,#0x2]
002c5724 add r12,r12,#0x1
002c5728 cmp r4,r12, lsl #0x1
002c572c bgt 0x002c5708
002c5730 ldr r2,[0x2c584c]
002c5734 ldr r3,[r2,#0x0]
002c5738 add r3,r3,r4, lsl #0x1
002c573c str r3,[r2,#0x0]
002c5740 b 0x002c5760
002c5744 ldr r2,[0x2c584c]
002c5748 ldr r3,[r2,#0x0]
002c574c add r3,r4,r3
002c5750 str r3,[r2,#0x0]
002c5754 cpy r1,r6
002c5758 cpy r2,r4
002c575c bl 0x00298c6c
002c5760 ldr r4,[0x2c584c]
002c5764 ldr r0,[r4,#0x0]
002c5768 mov r1,#0x0
002c576c bl 0x00294f30
002c5770 mov r3,#0x0
002c5774 str r3,[r4,#0x0]
002c5778 bl 0x00292d44
002c577c mov r2,#0x1
002c5780 ldr r3,[0x2c583c]
002c5784 str r2,[r3,#0x0]
002c5788 ldmia sp,{r4,r5,r6,r11,sp,pc}
002c578c ldr r3,[0x2c5848]
002c5790 ldr r3,[r3,#0x0]
002c5794 cmp r3,#0x1
002c5798 moveq r5,r1, lsl #0x1
002c579c cpyne r5,r1
002c57a0 cpy r0,r5
002c57a4 mov r1,#0x1
002c57a8 bl 0x00294ab0
002c57ac cmp r0,#0x0
002c57b0 ldmiale sp,{r4,r5,r6,r11,sp,pc}
002c57b4 cpy lr,r6
002c57b8 mov r12,#0x0
002c57bc ldr r3,[0x2c5848]
002c57c0 ldr r3,[r3,#0x0]
002c57c4 cmp r3,#0x1
002c57c8 bne 0x002c57f8
002c57cc mov r3,r12, lsl #0x2
002c57d0 add r1,r3,r0
002c57d4 mov r2,r12, lsl #0x1
002c57d8 ldrh r6,[r2,lr]
002c57dc strh r6,[r3,r0]
002c57e0 ldrh r2,[r2,lr]
002c57e4 strh r2,[r1,#0x2]
002c57e8 add r12,r12,#0x1
002c57ec cmp r4,r12, lsl #0x1
002c57f0 bgt 0x002c57cc
002c57f4 b 0x002c5804
002c57f8 cpy r1,r6
002c57fc cpy r2,r5
002c5800 bl 0x00298c6c
002c5804 cmp r4,#0x0
002c5808 bgt 0x002c581c
002c580c cpy r0,r5
002c5810 mov r1,#0x0
002c5814 bl 0x00294f30
002c5818 ldmia sp,{r4,r5,r6,r11,sp,pc}
002c581c cpy r0,r5
002c5820 mov r1,#0x0
002c5824 bl 0x00294f30
002c5828 bl 0x00292fd8
002c582c cmp r0,#0x4
002c5830 ldmiane sp,{r4,r5,r6,r11,sp,pc}
002c5834 bl 0x0029307c
002c5838 ldmia sp,{r4,r5,r6,r11,sp,pc}
REFERENCES TO ENTRY:
0045fa4c UNCONDITIONAL_CALL
00462810 UNCONDITIONAL_CALL
00462850 UNCONDITIONAL_CALL
