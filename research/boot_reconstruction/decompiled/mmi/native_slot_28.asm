002de8c0 ldr r1,[0x2de8ec]
002de8c4 ldrh r3,[r1,#0x0]
002de8c8 ldrh r2,[r1,#0x2]
002de8cc add r3,r3,r2, lsl #0x10
002de8d0 str r3,[r0,#0x0]
002de8d4 ldrh r3,[r1,#0x4]
002de8d8 ldrh r2,[r1,#0x6]
002de8dc add r3,r3,r2, lsl #0x10
002de8e0 str r3,[r0,#0x4]
002de8e4 mov r0,#0x0
002de8e8 bx lr
REFERENCES TO ENTRY:
002d33d0 DATA
