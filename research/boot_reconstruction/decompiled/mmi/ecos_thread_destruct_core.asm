0033a57c cpy r12,sp
0033a580 stmdb sp!,{r4,r11,r12,lr,pc}
0033a584 sub r11,r12,#0x4
0033a588 cpy r4,r0
0033a58c ldr r0,[0x33a5b4]
0033a590 cpy r1,r4
0033a594 bl 0x0033bcc8
0033a598 mov r3,#0x0
0033a59c strh r3,[r4,#0x48]
0033a5a0 add r0,r4,#0x4c
0033a5a4 bl 0x003395c8
0033a5a8 add r0,r4,#0x1c
0033a5ac bl 0x004ceae8
0033a5b0 ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
00339b4c UNCONDITIONAL_CALL
