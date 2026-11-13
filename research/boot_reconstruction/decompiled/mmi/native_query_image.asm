002ddb80 cpy r12,r2
002ddb84 and r0,r0,#0xff
002ddb88 and r1,r1,#0xff
002ddb8c cmp r0,#0x1
002ddb90 bls 0x002ddba4
002ddb94 ldr r3,[0x2ddc38]
002ddb98 ldrh r3,[r3,#0x0]
002ddb9c cmp r3,r0
002ddba0 bhi 0x002ddbb0
002ddba4 mov r0,#0x400
002ddba8 add r0,r0,#0x6
002ddbac bx lr
002ddbb0 add r2,r0,r0, lsl #0x3
002ddbb4 ldr r3,[0x2ddc3c]
002ddbb8 ldr r3,[r3,#0x0]
002ddbbc add r0,r3,r2, lsl #0x2
002ddbc0 ldrb r3,[r0,#-0x24]!
002ddbc4 cmp r3,#0x0
002ddbc8 moveq r0,#0x400
002ddbcc addeq r0,r0,#0x6
002ddbd0 bxeq lr
002ddbd4 sub r3,r1,#0x1
002ddbd8 cmp r3,#0x3
002ddbdc ldrls pc,[pc,r3,lsl #0x2]
002ddbe0 b 0x002ddc24
002ddbf4 ldrh r0,[r0,#0x8]
002ddbf8 strh r0,[r12,#0x0]
002ddbfc b 0x002ddc30
002ddc00 ldrh r0,[r0,#0xa]
002ddc04 strh r0,[r12,#0x0]
002ddc08 b 0x002ddc30
002ddc0c ldrh r0,[r0,#0x4]
002ddc10 strh r0,[r12,#0x0]
002ddc14 b 0x002ddc30
002ddc18 ldrb r3,[r0,#0x11]
002ddc1c strh r3,[r12,#0x0]
002ddc20 b 0x002ddc30
002ddc24 mov r0,#0x400
002ddc28 add r0,r0,#0x6
002ddc2c bx lr
002ddc30 mov r0,#0x0
002ddc34 bx lr
REFERENCES TO ENTRY:
002d3438 DATA
