002d3254 cpy r12,sp
002d3258 stmdb sp!,{r4,r11,r12,lr,pc}
002d325c sub r11,r12,#0x4
002d3260 bl 0x00339d18
002d3264 ldr r3,[0x2d3298]
002d3268 ldr r3,[r3,#0x0]
002d326c cpy r2,r1
002d3270 cpy r1,r0
002d3274 subs r1,r1,r3
002d3278 sbc r2,r2,#0x0
002d327c mov r0,r2, lsl #0x2
002d3280 orr r4,r0,r1, lsr #0x1e
002d3284 mov r3,r1, lsl #0x2
002d3288 adds r3,r3,r1
002d328c adc r4,r4,r2
002d3290 mov r0,r3, lsl #0x1
002d3294 ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
002d34f0 DATA
