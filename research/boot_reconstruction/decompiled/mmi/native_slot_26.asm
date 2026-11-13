002de848 ldr r1,[0x2de874]
002de84c ldrh r3,[r1,#0x0]
002de850 ldrh r2,[r1,#0x2]
002de854 add r3,r3,r2, lsl #0x10
002de858 str r3,[r0,#0x0]
002de85c ldrh r3,[r1,#0x4]
002de860 ldrh r2,[r1,#0x6]
002de864 add r3,r3,r2, lsl #0x10
002de868 str r3,[r0,#0x4]
002de86c mov r0,#0x0
002de870 bx lr
REFERENCES TO ENTRY:
002d33c0 DATA
