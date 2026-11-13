002e6004 cpy r12,sp
002e6008 stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr,pc}
002e600c sub r11,r12,#0x4
002e6010 sub sp,sp,#0x14
002e6014 str r0,[r11,#-0x2c]
002e6018 cpy r7,r1
002e601c cpy r10,r2
002e6020 str r3,[r11,#-0x30]
002e6024 ldr r9,[r11,#0x4]
002e6028 ldr r8,[r11,#0x8]
002e602c ldr r5,[r11,#0xc]
002e6030 mov r6,#0x0
002e6034 str r6,[r11,#-0x34]
002e6038 str r6,[r11,#-0x38]
002e603c str r6,[r11,#-0x3c]
002e6040 ldr r4,[0x2e65c0]
002e6044 mov r0,#0x1
002e6048 strh r0,[r4,#0x0]
002e604c tst r1,#0x1
002e6050 beq 0x002e6068
002e6054 ldr r0,[0x2e65c4]
002e6058 bl 0x00298b64
002e605c strh r6,[r4,#0x0]
002e6060 cpy r0,r6
002e6064 b 0x002e65b8
002e6068 ldr r1,[r11,#0x10]
002e606c tst r1,#0x3
002e6070 beq 0x002e6090
002e6074 ldr r0,[0x2e65c8]
002e6078 bl 0x00298b64
002e607c mov r2,#0x0
002e6080 ldr r3,[0x2e65c0]
002e6084 strh r2,[r3,#0x0]
002e6088 cpy r0,r2
002e608c b 0x002e65b8
002e6090 cmp r5,#0x0
002e6094 beq 0x002e60dc
002e6098 ldrh r3,[r5,#0x0]
002e609c tst r3,#0x1
002e60a0 beq 0x002e60b4
002e60a4 ldr r0,[0x2e65cc]
002e60a8 bl 0x00298b64
002e60ac mov r0,#0x0
002e60b0 b 0x002e65b8
002e60b4 ldrh r3,[r5,#0x4]
002e60b8 tst r3,#0x1
002e60bc beq 0x002e60dc
002e60c0 ldr r0,[0x2e65d0]
002e60c4 bl 0x00298b64
002e60c8 mov r2,#0x0
002e60cc ldr r3,[0x2e65c0]
002e60d0 strh r2,[r3,#0x0]
002e60d4 cpy r0,r2
002e60d8 b 0x002e65b8
002e60dc cmp r9,#0x320
002e60e0 bls 0x002e6100
002e60e4 mov r4,#0x0
002e60e8 ldr r3,[0x2e65c0]
002e60ec strh r4,[r3,#0x0]
002e60f0 ldr r0,[0x2e65d4]
002e60f4 bl 0x00298b64
002e60f8 cpy r0,r4
002e60fc b 0x002e65b8
002e6100 cmp r5,#0x0
002e6104 bne 0x002e615c
002e6108 cmp r9,r7
002e610c movls r3,#0x0
002e6110 movhi r3,#0x1
002e6114 cmp r8,r10
002e6118 movcs r3,#0x0
002e611c cmp r3,#0x0
002e6120 beq 0x002e6130
002e6124 ldr r0,[0x2e65d8]
002e6128 bl 0x00298b64
002e612c cpy r10,r8
002e6130 cmp r9,r7
002e6134 movcs r3,#0x0
002e6138 movcc r3,#0x1
002e613c cmp r8,r10
002e6140 movls r3,#0x0
002e6144 cmp r3,#0x0
002e6148 beq 0x002e61a4
002e614c ldr r0,[0x2e65dc]
002e6150 bl 0x00298b64
002e6154 cpy r8,r10
002e6158 b 0x002e61a4
002e615c ldrh r3,[r5,#0x4]
002e6160 cmp r3,r9
002e6164 bcs 0x002e6180
002e6168 ldrh r3,[r5,#0x6]
002e616c cmp r3,r8
002e6170 bls 0x002e6180
002e6174 ldr r0,[0x2e65d8]
002e6178 bl 0x00298b64
002e617c strh r8,[r5,#0x6]
002e6180 ldrh r3,[r5,#0x4]
002e6184 cmp r3,r9
002e6188 bls 0x002e61a4
002e618c ldrh r3,[r5,#0x6]
002e6190 cmp r3,r8
002e6194 bcs 0x002e61a4
002e6198 ldr r0,[0x2e65dc]
002e619c bl 0x00298b64
002e61a0 ldrh r8,[r5,#0x6]
002e61a4 cmp r5,#0x0
002e61a8 bne 0x002e620c
002e61ac mov r3,#0x92000000
002e61b0 add r3,r3,#0x7000
002e61b4 add r3,r3,#0x24
002e61b8 ldr r2,[r11,#0x10]
002e61bc str r2,[r3,#0x0]
002e61c0 mov r3,#0x92000000
002e61c4 add r3,r3,#0x7000
002e61c8 add r3,r3,#0x3c
002e61cc ldr r0,[r11,#0x14]
002e61d0 str r0,[r3,#0x0]
002e61d4 mov r3,#0x92000000
002e61d8 add r3,r3,#0x7000
002e61dc add r3,r3,#0x40
002e61e0 ldr r1,[r11,#0x18]
002e61e4 str r1,[r3,#0x0]
002e61e8 mov r3,#0x92000000
002e61ec add r3,r3,#0x7000
002e61f0 add r3,r3,#0x1c
002e61f4 str r5,[r3,#0x0]
002e61f8 mov r3,#0x92000000
002e61fc add r3,r3,#0x7000
002e6200 add r3,r3,#0x20
002e6204 str r5,[r3,#0x0]
002e6208 b 0x002e6384
002e620c ldrh r3,[r5,#0x0]
002e6210 tst r3,#0x1
002e6214 bne 0x002e6288
002e6218 ldrsh r2,[r5,#0x2]
002e621c ldrh r1,[r5,#0x6]
002e6220 add r3,r2,r1
002e6224 cmp r3,r10
002e6228 bhi 0x002e624c
002e622c mul r3,r7,r2
002e6230 ldr r2,[r11,#-0x34]
002e6234 add r2,r2,r3
002e6238 str r2,[r11,#-0x34]
002e623c ldr r0,[r11,#-0x38]
002e6240 add r0,r0,r3, lsr #0x2
002e6244 str r0,[r11,#-0x38]
002e6248 cpy r10,r1
002e624c ldrsh r2,[r5,#0x0]
002e6250 ldrh r1,[r5,#0x4]
002e6254 add r3,r2,r1
002e6258 cmp r3,r7
002e625c bhi 0x002e6288
002e6260 ldr r3,[r11,#-0x34]
002e6264 add r3,r3,r2
002e6268 str r3,[r11,#-0x34]
002e626c add r3,r2,r2, lsr #0x1f
002e6270 ldr r0,[r11,#-0x38]
002e6274 add r0,r0,r3, asr #0x1
002e6278 str r0,[r11,#-0x38]
002e627c rsb r7,r1,r7
002e6280 str r7,[r11,#-0x3c]
002e6284 cpy r7,r1
002e6288 ldr r1,[r11,#-0x2c]
002e628c cmp r1,#0x6
002e6290 beq 0x002e62f0
002e6294 bhi 0x002e62a4
002e6298 cmp r1,#0x3
002e629c beq 0x002e62d0
002e62a0 b 0x002e62b0
002e62a4 ldr r2,[r11,#-0x2c]
002e62a8 cmp r2,#0x9
002e62ac beq 0x002e62f0
002e62b0 ldr r3,[r11,#-0x34]
002e62b4 mov r3,r3, lsl #0x1
002e62b8 str r3,[r11,#-0x34]
002e62bc ldr r0,[r11,#-0x3c]
002e62c0 mov r0,r0, lsl #0x1
002e62c4 str r0,[r11,#-0x3c]
002e62c8 mov r6,r7, lsl #0x1
002e62cc b 0x002e62f4
002e62d0 ldr r1,[r11,#-0x34]
002e62d4 add r1,r1,r1, lsl #0x1
002e62d8 str r1,[r11,#-0x34]
002e62dc ldr r3,[r11,#-0x3c]
002e62e0 add r3,r3,r3, lsl #0x1
002e62e4 str r3,[r11,#-0x3c]
002e62e8 add r6,r7,r7, lsl #0x1
002e62ec b 0x002e62f4
002e62f0 cpy r6,r7
002e62f4 mov r3,#0x92000000
002e62f8 add r3,r3,#0x7000
002e62fc add r3,r3,#0x18
002e6300 str r6,[r3,#0x0]
002e6304 mov r3,#0x92000000
002e6308 add r3,r3,#0x7000
002e630c add r3,r3,#0x1c
002e6310 ldr r1,[r11,#-0x3c]
002e6314 str r1,[r3,#0x0]
002e6318 mov r3,#0x92000000
002e631c add r3,r3,#0x7000
002e6320 add r3,r3,#0x20
002e6324 mov r2,r1, lsr #0x1
002e6328 str r2,[r3,#0x0]
002e632c ldr r3,[0x2e65e0]
002e6330 ldr r2,[r11,#-0x34]
002e6334 str r2,[r3,#0x0]
002e6338 mov r3,#0x92000000
002e633c add r3,r3,#0x7000
002e6340 add r3,r3,#0x24
002e6344 ldr r0,[r11,#0x10]
002e6348 add r2,r0,r2
002e634c str r2,[r3,#0x0]
002e6350 mov r3,#0x92000000
002e6354 add r3,r3,#0x7000
002e6358 add r3,r3,#0x3c
002e635c ldr r0,[r11,#0x14]
002e6360 ldr r1,[r11,#-0x38]
002e6364 add r2,r0,r1
002e6368 str r2,[r3,#0x0]
002e636c mov r3,#0x92000000
002e6370 add r3,r3,#0x7000
002e6374 add r3,r3,#0x40
002e6378 ldr r0,[r11,#0x18]
002e637c add r2,r0,r1
002e6380 str r2,[r3,#0x0]
002e6384 mov r5,#0x92000000
002e6388 add r5,r5,#0x7000
002e638c add r5,r5,#0x4
002e6390 ldr r6,[r5,#0x0]
002e6394 bic r6,r6,#0x1140
002e6398 bic r6,r6,#0x37
002e639c ldr r0,[r11,#-0x2c]
002e63a0 bl 0x002e5c94
002e63a4 cpy r4,r0
002e63a8 ldr r0,[r11,#-0x30]
002e63ac bl 0x002e5c94
002e63b0 orr r4,r4,r0, lsl #0x4
002e63b4 orr r6,r6,r4
002e63b8 str r6,[r5,#0x0]
002e63bc mov r6,r7, lsl #0x13
002e63c0 mov r6,r6, lsr #0x13
002e63c4 mov r3,#0x1fc0
002e63c8 add r3,r3,#0x3f
002e63cc cmp r7,r6
002e63d0 cpyne r7,r3
002e63d4 mov r3,#0x92000000
002e63d8 add r3,r3,#0x7000
002e63dc add r3,r3,#0x8
002e63e0 str r7,[r3,#0x0]
002e63e4 mov r3,#0x92000000
002e63e8 add r3,r3,#0x7000
002e63ec add r3,r3,#0x4
002e63f0 ldr r6,[r3,#0x0]
002e63f4 bic r6,r6,#0xf8
002e63f8 mov r6,r6, lsl #0x17
002e63fc mov r6,r6, lsr #0x17
002e6400 cmp r6,#0x2
002e6404 bne 0x002e6420
002e6408 mov r3,#0x92000000
002e640c add r3,r3,#0x7000
002e6410 add r3,r3,#0x18
002e6414 add r2,r7,r7, lsl #0x1
002e6418 str r2,[r3,#0x0]
002e641c b 0x002e6454
002e6420 mov r3,#0x104
002e6424 add r3,r3,#0x2
002e6428 cmp r6,r3
002e642c cmpne r6,#0x6
002e6430 moveq r3,#0x92000000
002e6434 addeq r3,r3,#0x7000
002e6438 addeq r3,r3,#0x18
002e643c streq r7,[r3,#0x0]
002e6440 movne r3,#0x92000000
002e6444 addne r3,r3,#0x7000
002e6448 addne r3,r3,#0x18
002e644c movne r2,r7, lsl #0x1
002e6450 strne r2,[r3,#0x0]
002e6454 mov r6,r10, lsl #0x13
002e6458 mov r6,r6, lsr #0x13
002e645c mov r3,#0x1fc0
002e6460 add r3,r3,#0x3f
002e6464 cmp r10,r6
002e6468 cpyne r10,r3
002e646c mov r3,#0x92000000
002e6470 add r3,r3,#0x7000
002e6474 add r3,r3,#0xc
002e6478 str r10,[r3,#0x0]
002e647c mov r6,r9, lsl #0x15
002e6480 mov r6,r6, lsr #0x15
002e6484 mov r3,#0x7f0
002e6488 add r3,r3,#0xf
002e648c cmp r9,r6
002e6490 cpyne r9,r3
002e6494 mov r3,#0x92000000
002e6498 add r3,r3,#0x7000
002e649c add r3,r3,#0x10
002e64a0 str r9,[r3,#0x0]
002e64a4 mov r6,r8, lsl #0x15
002e64a8 mov r6,r6, lsr #0x15
002e64ac mov r3,#0x7f0
002e64b0 add r3,r3,#0xf
002e64b4 cmp r8,r6
002e64b8 cpyne r8,r3
002e64bc mov r3,#0x92000000
002e64c0 add r3,r3,#0x7000
002e64c4 add r3,r3,#0x14
002e64c8 str r8,[r3,#0x0]
002e64cc cmp r7,r9
002e64d0 beq 0x002e6518
002e64d4 cmp r9,r7
002e64d8 bls 0x002e64f0
002e64dc mov r0,r7, lsl #0x10
002e64e0 cpy r1,r9
002e64e4 bl 0x00333f98
002e64e8 orr r6,r0,#0x10000
002e64ec b 0x002e6504
002e64f0 add r6,r7,r9, lsl #0xc
002e64f4 sub r0,r6,#0x1
002e64f8 cpy r1,r7
002e64fc bl 0x00333f98
002e6500 orr r6,r0,#0x100000
002e6504 mov r3,#0x92000000
002e6508 add r3,r3,#0x7000
002e650c add r3,r3,#0x2c
002e6510 str r6,[r3,#0x0]
002e6514 b 0x002e652c
002e6518 mov r6,#0x0
002e651c mov r3,#0x92000000
002e6520 add r3,r3,#0x7000
002e6524 add r3,r3,#0x2c
002e6528 str r6,[r3,#0x0]
002e652c cmp r10,r8
002e6530 beq 0x002e65a0
002e6534 cmp r8,r10
002e6538 bls 0x002e6550
002e653c mov r0,r10, lsl #0x10
002e6540 cpy r1,r8
002e6544 bl 0x00333f98
002e6548 orr r6,r0,#0x10000
002e654c b 0x002e658c
002e6550 mov r4,r8, lsl #0xc
002e6554 add r6,r4,r10
002e6558 sub r0,r6,#0x1
002e655c cpy r1,r10
002e6560 bl 0x00333f98
002e6564 cpy r6,r0
002e6568 add r0,r4,r0
002e656c sub r0,r0,#0x1
002e6570 cpy r1,r6
002e6574 bl 0x00333f98
002e6578 mov r3,#0x92000000
002e657c add r3,r3,#0x7000
002e6580 add r3,r3,#0xc
002e6584 str r0,[r3,#0x0]
002e6588 orr r6,r6,#0x100000
002e658c mov r3,#0x92000000
002e6590 add r3,r3,#0x7000
002e6594 add r3,r3,#0x30
002e6598 str r6,[r3,#0x0]
002e659c b 0x002e65b4
002e65a0 mov r6,#0x0
002e65a4 mov r3,#0x92000000
002e65a8 add r3,r3,#0x7000
002e65ac add r3,r3,#0x30
002e65b0 str r6,[r3,#0x0]
002e65b4 mov r0,#0x1
002e65b8 sub sp,r11,#0x28
002e65bc ldmia sp,{r4,r5,r6,r7,r8,r9,r10,r11,sp,pc}
REFERENCES TO ENTRY:
00446278 UNCONDITIONAL_CALL
004438f0 UNCONDITIONAL_CALL
002e6dc0 UNCONDITIONAL_CALL
002e6df4 UNCONDITIONAL_CALL
002e6e44 UNCONDITIONAL_CALL
002e7034 UNCONDITIONAL_CALL
002e7074 UNCONDITIONAL_CALL
002e5f00 UNCONDITIONAL_CALL
002d72d4 UNCONDITIONAL_CALL
002defd8 UNCONDITIONAL_CALL
002ea9c0 UNCONDITIONAL_CALL
002eaf24 UNCONDITIONAL_CALL
002eb010 UNCONDITIONAL_CALL
00337034 UNCONDITIONAL_CALL
00337aac UNCONDITIONAL_CALL
003386e8 UNCONDITIONAL_CALL
004448d4 UNCONDITIONAL_CALL
00444930 UNCONDITIONAL_CALL
00444a08 UNCONDITIONAL_CALL
00445068 UNCONDITIONAL_CALL
00463bf8 UNCONDITIONAL_CALL
00463c4c UNCONDITIONAL_CALL
