00040ed4 stmdb sp!,{r4,r5,lr}
00040ed8 cpy r5,r0
00040edc cpy r4,r1
00040ee0 ldr r1,[r0,#0x24]
00040ee4 bl 0x000409e4
00040ee8 mov r4,r4, lsl #0x4
00040eec add r4,r4,r0
00040ef0 cpy r0,r5
00040ef4 ldrh r1,[r4,#0xe]
00040ef8 bl 0x000409e4
00040efc ldr r3,[r4,#0x4]
00040f00 add r0,r0,r3
00040f04 ldmia sp!,{r4,r5,pc}
REFERENCES TO ENTRY:
00041010 UNCONDITIONAL_CALL
00041034 UNCONDITIONAL_CALL
