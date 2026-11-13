002de124 cpy r12,sp
002de128 stmdb sp!,{r4,r5,r11,r12,lr,pc}
002de12c sub r11,r12,#0x4
002de130 cpy r4,r0
002de134 cpy r5,r1
002de138 ldr r3,[0x2de160]
002de13c mov lr,pc
002de140 ldr pc,[r3,#0x0]
002de144 strh r0,[r4,#0x0]
002de148 ldr r3,[0x2de164]
002de14c mov lr,pc
002de150 ldr pc,[r3,#0x0]
002de154 strh r0,[r5,#0x0]
002de158 mov r0,#0x0
002de15c ldmia sp,{r4,r5,r11,sp,pc}
REFERENCES TO ENTRY:
002d3418 DATA
