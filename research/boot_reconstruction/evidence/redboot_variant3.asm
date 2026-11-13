00036ab4: e92d45f0 push     {r4, r5, r6, r7, r8, sl, lr}
00036ab8: e24dd04c sub      sp, sp, #0x4c
00036abc: e3a01001 mov      r1, #1
00036ac0: eb002366 bl       #0x3f860
00036ac4: e2506000 subs     r6, r0, #0
00036ac8: aa000003 bge      #0x36adc
00036acc: e59f0110 ldr      r0, [pc, #0x110] ; [00036be4] = 00050c6c 
00036ad0: eb003ad9 bl       #0x4563c
00036ad4: e3e0a000 mvn      sl, #0
00036ad8: ea00003e b        #0x36bd8
00036adc: e28d403c add      r4, sp, #0x3c
00036ae0: e1a00006 mov      r0, r6
00036ae4: e1a01004 mov      r1, r4
00036ae8: e3a02010 mov      r2, #0x10
00036aec: eb002433 bl       #0x3fbc0
00036af0: e59d5040 ldr      r5, [sp, #0x40]
00036af4: e1a00004 mov      r0, r4
00036af8: e59f10e8 ldr      r1, [pc, #0xe8] ; [00036be8] = 69504d4d 
00036afc: ebffff4d bl       #0x36838
00036b00: e3500000 cmp      r0, #0
00036b04: 0a00002b beq      #0x36bb8
00036b08: e59f00dc ldr      r0, [pc, #0xdc] ; [00036bec] = 00050c80 
00036b0c: eb003aca bl       #0x4563c
00036b10: e3e0a000 mvn      sl, #0
00036b14: ea00002f b        #0x36bd8
00036b18: e1a00006 mov      r0, r6
00036b1c: e28d1018 add      r1, sp, #0x18
00036b20: e3a02024 mov      r2, #0x24
00036b24: eb002425 bl       #0x3fbc0
00036b28: e2855024 add      r5, r5, #0x24
00036b2c: e59d3028 ldr      r3, [sp, #0x28]
00036b30: e313080f tst      r3, #0xf0000
00036b34: 1a000002 bne      #0x36b44
00036b38: e59d3030 ldr      r3, [sp, #0x30]
00036b3c: e3730001 cmn      r3, #1
00036b40: 11a0a003 movne    sl, r3
00036b44: e59d302c ldr      r3, [sp, #0x2c]
00036b48: e3530000 cmp      r3, #0
00036b4c: 0a000016 beq      #0x36bac
00036b50: e3a08000 mov      r8, #0
00036b54: e1a0400d mov      r4, sp
00036b58: e1a00006 mov      r0, r6
00036b5c: e1a0100d mov      r1, sp
00036b60: e3a02018 mov      r2, #0x18
00036b64: eb002415 bl       #0x3fbc0
00036b68: e1a0000d mov      r0, sp
00036b6c: e59f107c ldr      r1, [pc, #0x7c] ; [00036bf0] = 73504d4d 
00036b70: ebffff30 bl       #0x36838
00036b74: e3500000 cmp      r0, #0
00036b78: 1a000007 bne      #0x36b9c
00036b7c: e59d4004 ldr      r4, [sp, #4]
00036b80: e2444018 sub      r4, r4, #0x18
00036b84: e2855018 add      r5, r5, #0x18
00036b88: e1a00006 mov      r0, r6
00036b8c: e59d1008 ldr      r1, [sp, #8]
00036b90: e1a02004 mov      r2, r4
00036b94: eb002409 bl       #0x3fbc0
00036b98: e0855004 add      r5, r5, r4
00036b9c: e2888001 add      r8, r8, #1
00036ba0: e59d302c ldr      r3, [sp, #0x2c]
00036ba4: e1530008 cmp      r3, r8
00036ba8: 8affffe9 bhi      #0x36b54
00036bac: e1570005 cmp      r7, r5
00036bb0: da000006 ble      #0x36bd0
00036bb4: eaffffd7 b        #0x36b18
00036bb8: e1a07005 mov      r7, r5
00036bbc: e3550010 cmp      r5, #0x10
00036bc0: c3a05010 movgt    r5, #0x10
00036bc4: c3a0a000 movgt    sl, #0
00036bc8: caffffd2 bgt      #0x36b18
00036bcc: e3a0a000 mov      sl, #0
00036bd0: e1a00006 mov      r0, r6
00036bd4: eb002402 bl       #0x3fbe4
00036bd8: e1a0000a mov      r0, sl
00036bdc: e28dd04c add      sp, sp, #0x4c
00036be0: e8bd85f0 pop      {r4, r5, r6, r7, r8, sl, pc}
00036be4: 00050c6c andeq    r0, r5, ip, ror #24
00036be8: 69504d4d ldmdbvs  r0, {r0, r2, r3, r6, r8, sl, fp, lr} ^
00036bec: 00050c80 andeq    r0, r5, r0, lsl #25
00036bf0: 73504d4d cmpvc    r0, #0x1340
00036bf4: e92d4070 push     {r4, r5, r6, lr}
00036bf8: e1a04000 mov      r4, r0
00036bfc: e1a06001 mov      r6, r1
00036c00: e59f0060 ldr      r0, [pc, #0x60] ; [00036c68] = 00050cac 
00036c04: eb003a8c bl       #0x4563c
00036c08: e1a00004 mov      r0, r4
00036c0c: e3a01001 mov      r1, #1
00036c10: eb002312 bl       #0x3f860
00036c14: e2505000 subs     r5, r0, #0
00036c18: ba00000d blt      #0x36c54
00036c1c: e1a04006 mov      r4, r6
00036c20: e1a00005 mov      r0, r5
00036c24: e1a01004 mov      r1, r4
00036c28: e3a02a01 mov      r2, #0x1000
00036c2c: eb0023e3 bl       #0x3fbc0
00036c30: e2844a01 add      r4, r4, #0x1000
00036c34: e3500000 cmp      r0, #0
00036c38: 1afffff8 bne      #0x36c20
00036c3c: e1a00005 mov      r0, r5
00036c40: eb0023e7 bl       #0x3fbe4
00036c44: e1a00006 mov      r0, r6
00036c48: e3a01001 mov      r1, #1
00036c4c: eb001eb8 bl       #0x3e734
00036c50: e8bd8070 pop      {r4, r5, r6, pc}
00036c54: e59f0010 ldr      r0, [pc, #0x10] ; [00036c6c] = 00050cc4 
00036c58: e1a01004 mov      r1, r4
00036c5c: eb003a76 bl       #0x4563c
00036c60: e3a00000 mov      r0, #0
00036c64: e8bd8070 pop      {r4, r5, r6, pc}
00036c68: 00050cac andeq    r0, r5, ip, lsr #25
00036c6c: 00050cc4 andeq    r0, r5, r4, asr #25
0003e3c0: e3120003 tst      r2, #3
0003e3c4: 112fff1e bxne     lr
0003e3c8: e1b02122 lsrs     r2, r2, #2
0003e3cc: 012fff1e bxeq     lr
0003e3d0: e4913004 ldr      r3, [r1], #4
0003e3d4: e4803004 str      r3, [r0], #4
0003e3d8: e2522001 subs     r2, r2, #1
0003e3dc: 1afffffb bne      #0x3e3d0
0003e3e0: e12fff1e bx       lr
0003e3e4: e5900000 ldr      r0, [r0]
0003e3e8: e0610000 rsb      r0, r1, r0
0003e3ec: e12fff1e bx       lr
0003e3f0: e92d41f0 push     {r4, r5, r6, r7, r8, lr}
0003e3f4: e24dd018 sub      sp, sp, #0x18
0003e3f8: e1a07001 mov      r7, r1
0003e3fc: e2508000 subs     r8, r0, #0
0003e400: 0a00001d beq      #0x3e47c
0003e404: e3510000 cmp      r1, #0
0003e408: 01a05001 moveq    r5, r1
0003e40c: 0a000018 beq      #0x3e474
0003e410: e3a05000 mov      r5, #0
0003e414: e1a06005 mov      r6, r5
0003e418: e1a0400d mov      r4, sp
0003e41c: e3c51003 bic      r1, r5, #3
0003e420: e1a0000d mov      r0, sp
0003e424: e0811008 add      r1, r1, r8
0003e428: e3a02018 mov      r2, #0x18
0003e42c: ebffffe3 bl       #0x3e3c0
0003e430: e1a0000d mov      r0, sp
0003e434: e59f104c ldr      r1, [pc, #0x4c] ; [0003e488] = 73504d4d 
0003e438: ebffffe9 bl       #0x3e3e4
0003e43c: e3500000 cmp      r0, #0
0003e440: 1a00000d bne      #0x3e47c
0003e444: e59d4004 ldr      r4, [sp, #4]
0003e448: e2444018 sub      r4, r4, #0x18
0003e44c: e2855018 add      r5, r5, #0x18
0003e450: e3c51003 bic      r1, r5, #3
0003e454: e59d0008 ldr      r0, [sp, #8]
0003e458: e0811008 add      r1, r1, r8
0003e45c: e1a02004 mov      r2, r4
0003e460: ebffffd6 bl       #0x3e3c0
0003e464: e0855004 add      r5, r5, r4
0003e468: e2866001 add      r6, r6, #1
0003e46c: e1560007 cmp      r6, r7
0003e470: 1affffe8 bne      #0x3e418
0003e474: e1a00005 mov      r0, r5
0003e478: ea000000 b        #0x3e480
0003e47c: e3e00000 mvn      r0, #0
0003e480: e28dd018 add      sp, sp, #0x18
0003e484: e8bd81f0 pop      {r4, r5, r6, r7, r8, pc}
0003e488: 73504d4d cmpvc    r0, #0x1340
0003e48c: e92d40f0 push     {r4, r5, r6, r7, lr}
0003e490: e1a05000 mov      r5, r0
0003e494: e1a03001 mov      r3, r1
0003e498: e3510000 cmp      r1, #0
0003e49c: 13500000 cmpne    r0, #0
0003e4a0: 0a000029 beq      #0x3e54c
0003e4a4: e2121003 ands     r1, r2, #3
0003e4a8: 1a000027 bne      #0x3e54c
0003e4ac: e1a06003 mov      r6, r3
0003e4b0: e1a0e222 lsr      lr, r2, #4
0003e4b4: e1a04001 mov      r4, r1
0003e4b8: e1a07001 mov      r7, r1
0003e4bc: e3170001 tst      r7, #1
0003e4c0: 1a000004 bne      #0x3e4d8
0003e4c4: e35e0000 cmp      lr, #0
0003e4c8: 11a0c006 movne    ip, r6
0003e4cc: 13a00000 movne    r0, #0
0003e4d0: 1a00000f bne      #0x3e514
0003e4d4: ea000015 b        #0x3e530
0003e4d8: e3170002 tst      r7, #2
0003e4dc: 11a03e02 lslne    r3, r2, #0x1c
0003e4e0: 108eef23 addne    lr, lr, r3, lsr #30
0003e4e4: e35e0000 cmp      lr, #0
0003e4e8: 0a000010 beq      #0x3e530
0003e4ec: e1a0c006 mov      ip, r6
0003e4f0: e3a00000 mov      r0, #0
0003e4f4: e49c3004 ldr      r3, [ip], #4
0003e4f8: e0844003 add      r4, r4, r3
0003e4fc: e2800001 add      r0, r0, #1
0003e500: e150000e cmp      r0, lr
0003e504: 1afffffa bne      #0x3e4f4
0003e508: e1a0310e lsl      r3, lr, #2
0003e50c: e0866003 add      r6, r6, r3
0003e510: ea000006 b        #0x3e530
0003e514: e49c3004 ldr      r3, [ip], #4
0003e518: e0811003 add      r1, r1, r3
0003e51c: e2800001 add      r0, r0, #1
0003e520: e150000e cmp      r0, lr
0003e524: 1afffffa bne      #0x3e514
0003e528: e1a0310e lsl      r3, lr, #2
0003e52c: e0866003 add      r6, r6, r3
0003e530: e2877001 add      r7, r7, #1
0003e534: e3570004 cmp      r7, #4
0003e538: 1affffdf bne      #0x3e4bc
0003e53c: e5851008 str      r1, [r5, #8]
0003e540: e585400c str      r4, [r5, #0xc]
0003e544: e3a00001 mov      r0, #1
0003e548: e8bd80f0 pop      {r4, r5, r6, r7, pc}
0003e54c: e3e00000 mvn      r0, #0
0003e550: e8bd80f0 pop      {r4, r5, r6, r7, pc}
0003e734: e92d47f0 push     {r4, r5, r6, r7, r8, sb, sl, lr}
0003e738: e24dd038 sub      sp, sp, #0x38
0003e73c: e1a09001 mov      sb, r1
0003e740: e1a05000 mov      r5, r0
0003e744: e28d4028 add      r4, sp, #0x28
0003e748: e1a00004 mov      r0, r4
0003e74c: e1a01005 mov      r1, r5
0003e750: e3a02010 mov      r2, #0x10
0003e754: ebffff19 bl       #0x3e3c0
0003e758: e59d702c ldr      r7, [sp, #0x2c]
0003e75c: e1a00004 mov      r0, r4
0003e760: e59f1144 ldr      r1, [pc, #0x144] ; [0003e8ac] = 68504d4d 
0003e764: ebffff1e bl       #0x3e3e4
0003e768: e3500000 cmp      r0, #0
0003e76c: 1a00001a bne      #0x3e7dc
0003e770: e2850010 add      r0, r5, #0x10
0003e774: e2471010 sub      r1, r7, #0x10
0003e778: eb0001b7 bl       #0x3ee5c
0003e77c: e250a000 subs     sl, r0, #0
0003e780: 11a03087 lslne    r3, r7, #1
0003e784: 10838005 addne    r8, r3, r5
0003e788: 11a06008 movne    r6, r8
0003e78c: 13a04010 movne    r4, #0x10
0003e790: 1a000003 bne      #0x3e7a4
0003e794: ea000016 b        #0x3e7f4
0003e798: e5dd3003 ldrb     r3, [sp, #3]
0003e79c: e7c63004 strb     r3, [r6, r4]
0003e7a0: e2844001 add      r4, r4, #1
0003e7a4: e28d0003 add      r0, sp, #3
0003e7a8: eb0001f0 bl       #0x3ef70
0003e7ac: e3500000 cmp      r0, #0
0003e7b0: 1afffff8 bne      #0x3e798
0003e7b4: e28a6010 add      r6, sl, #0x10
0003e7b8: e1560004 cmp      r6, r4
0003e7bc: 1a00000c bne      #0x3e7f4
0003e7c0: e1a00008 mov      r0, r8
0003e7c4: e1a01005 mov      r1, r5
0003e7c8: e3a02010 mov      r2, #0x10
0003e7cc: ebfffefb bl       #0x3e3c0
0003e7d0: e1a07006 mov      r7, r6
0003e7d4: e1a05008 mov      r5, r8
0003e7d8: ea000005 b        #0x3e7f4
0003e7dc: e1a00004 mov      r0, r4
0003e7e0: e59f10c8 ldr      r1, [pc, #0xc8] ; [0003e8b0] = 69504d4d 
0003e7e4: ebfffefe bl       #0x3e3e4
0003e7e8: e3500000 cmp      r0, #0
0003e7ec: 13e06000 mvnne    r6, #0
0003e7f0: 1a00002a bne      #0x3e8a0
0003e7f4: e59d4030 ldr      r4, [sp, #0x30]
0003e7f8: e59d6034 ldr      r6, [sp, #0x34]
0003e7fc: e3190001 tst      sb, #1
0003e800: 0a000009 beq      #0x3e82c
0003e804: e28d0028 add      r0, sp, #0x28
0003e808: e2851010 add      r1, r5, #0x10
0003e80c: e2472010 sub      r2, r7, #0x10
0003e810: ebffff1d bl       #0x3e48c
0003e814: e59d3030 ldr      r3, [sp, #0x30]
0003e818: e1530004 cmp      r3, r4
0003e81c: 1a00001e bne      #0x3e89c
0003e820: e59d3034 ldr      r3, [sp, #0x34]
0003e824: e1530006 cmp      r3, r6
0003e828: 1a00001b bne      #0x3e89c
0003e82c: e3570010 cmp      r7, #0x10
0003e830: d3a06000 movle    r6, #0
0003e834: da000019 ble      #0x3e8a0
0003e838: e3a04010 mov      r4, #0x10
0003e83c: e3a06000 mov      r6, #0
0003e840: e3c41003 bic      r1, r4, #3
0003e844: e28d0004 add      r0, sp, #4
0003e848: e0811005 add      r1, r1, r5
0003e84c: e3a02024 mov      r2, #0x24
0003e850: ebfffeda bl       #0x3e3c0
0003e854: e2844024 add      r4, r4, #0x24
0003e858: e59d3014 ldr      r3, [sp, #0x14]
0003e85c: e313080f tst      r3, #0xf0000
0003e860: 1a000002 bne      #0x3e870
0003e864: e59d301c ldr      r3, [sp, #0x1c]
0003e868: e3730001 cmn      r3, #1
0003e86c: 11a06003 movne    r6, r3
0003e870: e3c40003 bic      r0, r4, #3
0003e874: e0800005 add      r0, r0, r5
0003e878: e59d1018 ldr      r1, [sp, #0x18]
0003e87c: ebfffedb bl       #0x3e3f0
0003e880: e3500000 cmp      r0, #0
0003e884: b3e06002 mvnlt    r6, #2
0003e888: ba000004 blt      #0x3e8a0
0003e88c: e0804004 add      r4, r0, r4
0003e890: e1570004 cmp      r7, r4
0003e894: da000001 ble      #0x3e8a0
0003e898: eaffffe8 b        #0x3e840
0003e89c: e3e06001 mvn      r6, #1
0003e8a0: e1a00006 mov      r0, r6
0003e8a4: e28dd038 add      sp, sp, #0x38
0003e8a8: e8bd87f0 pop      {r4, r5, r6, r7, r8, sb, sl, pc}
0003e8ac: 68504d4d ldmdavs  r0, {r0, r2, r3, r6, r8, sl, fp, lr} ^
0003e8b0: 69504d4d ldmdbvs  r0, {r0, r2, r3, r6, r8, sl, fp, lr} ^
00045bdc: e92d4010 push     {r4, lr}
00045be0: e10f3000 mrs      r3, apsr
00045be4: e10f4000 mrs      r4, apsr
00045be8: e38440c0 orr      r4, r4, #0xc0
00045bec: e129f004 msr      cpsr_fc, r4
00045bf0: e12fff30 blx      r0
00045bf4: e59f0000 ldr      r0, [pc] ; [00045bfc] = 0000d53c 
00045bf8: eb0013c7 bl       #0x4ab1c
00045bfc: 0000d53c andeq    sp, r0, ip, lsr r5
00046220: e92d41f0 push     {r4, r5, r6, r7, r8, lr}
00046224: e24dd0b0 sub      sp, sp, #0xb0
00046228: e1a06000 mov      r6, r0
0004622c: e1a07001 mov      r7, r1
00046230: e3a04000 mov      r4, #0
00046234: e58d40a0 str      r4, [sp, #0xa0]
00046238: e58d409c str      r4, [sp, #0x9c]
0004623c: e58d4090 str      r4, [sp, #0x90]
00046240: e59f24d0 ldr      r2, [pc, #0x4d0] ; [00046718] = 9d801000 
00046244: e3a030aa mov      r3, #0xaa
00046248: e5823000 str      r3, [r2]
0004624c: e2433055 sub      r3, r3, #0x55
00046250: e5823004 str      r3, [r2, #4]
00046254: e59f04c0 ldr      r0, [pc, #0x4c0] ; [0004671c] = 00054ad8 
00046258: ebfffcf7 bl       #0x4563c
0004625c: e59f34bc ldr      r3, [pc, #0x4bc] ; [00046720] = 00009244 
00046260: e5834000 str      r4, [r3]
00046264: e59f34b8 ldr      r3, [pc, #0x4b8] ; [00046724] = 00017208 
00046268: e5933000 ldr      r3, [r3]
0004626c: e58d30ac str      r3, [sp, #0xac]
00046270: e28d5018 add      r5, sp, #0x18
00046274: e28d30a4 add      r3, sp, #0xa4
00046278: e58d3000 str      r3, [sp]
0004627c: e28d30a8 add      r3, sp, #0xa8
00046280: e58d3004 str      r3, [sp, #4]
00046284: e59f349c ldr      r3, [pc, #0x49c] ; [00046728] = 00054ae0 'wait timeout'
00046288: e58d3008 str      r3, [sp, #8]
0004628c: e1a00005 mov      r0, r5
00046290: e3a01077 mov      r1, #0x77
00046294: e3a02001 mov      r2, #1
00046298: e1a03004 mov      r3, r4
0004629c: eb000731 bl       #0x47f68
000462a0: e28d30a0 add      r3, sp, #0xa0
000462a4: e88d0018 stm      sp, {r3, r4}
000462a8: e59f347c ldr      r3, [pc, #0x47c] ; [0004672c] = 00054af0 'go with caches enabled'
000462ac: e58d3008 str      r3, [sp, #8]
000462b0: e28d0030 add      r0, sp, #0x30
000462b4: e3a01063 mov      r1, #0x63
000462b8: e1a02004 mov      r2, r4
000462bc: e3a03002 mov      r3, #2
000462c0: eb000728 bl       #0x47f68
000462c4: e28d3090 add      r3, sp, #0x90
000462c8: e88d0018 stm      sp, {r3, r4}
000462cc: e59f345c ldr      r3, [pc, #0x45c] ; [00046730] = 00054b08 'go with network driver stopped'
000462d0: e58d3008 str      r3, [sp, #8]
000462d4: e28d0048 add      r0, sp, #0x48
000462d8: e3a0106e mov      r1, #0x6e
000462dc: e1a02004 mov      r2, r4
000462e0: e3a03002 mov      r3, #2
000462e4: eb00071f bl       #0x47f68
000462e8: e28d309c add      r3, sp, #0x9c
000462ec: e58d3000 str      r3, [sp]
000462f0: e28d3098 add      r3, sp, #0x98
000462f4: e58d3004 str      r3, [sp, #4]
000462f8: e59f3434 ldr      r3, [pc, #0x434] ; [00046734] = 00054b28 'play rmvb/mpeg4/h264'
000462fc: e58d3008 str      r3, [sp, #8]
00046300: e28d0060 add      r0, sp, #0x60
00046304: e3a01072 mov      r1, #0x72
00046308: e3a02001 mov      r2, #1
0004630c: e1a03004 mov      r3, r4
00046310: eb000714 bl       #0x47f68
00046314: e28d3094 add      r3, sp, #0x94
00046318: e88d0018 stm      sp, {r3, r4}
0004631c: e59f3414 ldr      r3, [pc, #0x414] ; [00046738] = 00054b40 'play on demo kit or evb'
00046320: e58d3008 str      r3, [sp, #8]
00046324: e28d0078 add      r0, sp, #0x78
00046328: e3a01064 mov      r1, #0x64
0004632c: e1a02004 mov      r2, r4
00046330: e3a03002 mov      r3, #2
00046334: eb00070b bl       #0x47f68
00046338: e3a03005 mov      r3, #5
0004633c: e58d3000 str      r3, [sp]
00046340: e28d30ac add      r3, sp, #0xac
00046344: e98d0018 stmib    sp, {r3, r4}
00046348: e59f33ec ldr      r3, [pc, #0x3ec] ; [0004673c] = 00054b58 'starting address'
0004634c: e58d300c str      r3, [sp, #0xc]
00046350: e1a00006 mov      r0, r6
00046354: e1a01007 mov      r1, r7
00046358: e3a02001 mov      r2, #1
0004635c: e1a03005 mov      r3, r5
00046360: eb000759 bl       #0x480cc
00046364: e1500004 cmp      r0, r4
00046368: 1a000007 bne      #0x4638c
0004636c: e59f33cc ldr      r3, [pc, #0x3cc] ; [00046740] = 00055010 
00046370: e5933000 ldr      r3, [r3]
00046374: e5933000 ldr      r3, [r3]
00046378: e3130001 tst      r3, #1
0004637c: 0a0000e3 beq      #0x46710
00046380: e59f03bc ldr      r0, [pc, #0x3bc] ; [00046744] = 00054b6c 
00046384: ebfffcac bl       #0x4563c
00046388: ea0000e0 b        #0x46710
0004638c: e59d30ac ldr      r3, [sp, #0xac]
00046390: e3730001 cmn      r3, #1
00046394: 1a000002 bne      #0x463a4
00046398: e59f03a8 ldr      r0, [pc, #0x3a8] ; [00046748] = 00054b80 
0004639c: ebfffca6 bl       #0x4563c
000463a0: ea0000da b        #0x46710
000463a4: e59f03a0 ldr      r0, [pc, #0x3a0] ; [0004674c] = 00054ba0 
000463a8: e59d109c ldr      r1, [sp, #0x9c]
000463ac: e59d2098 ldr      r2, [sp, #0x98]
000463b0: ebfffca1 bl       #0x4563c
000463b4: e59d209c ldr      r2, [sp, #0x9c]
000463b8: e3520000 cmp      r2, #0
000463bc: 0a000002 beq      #0x463cc
000463c0: e59d3098 ldr      r3, [sp, #0x98]
000463c4: e3530000 cmp      r3, #0
000463c8: 1a000005 bne      #0x463e4
000463cc: e59f237c ldr      r2, [pc, #0x37c] ; [00046750] = 0000aa55 
000463d0: e59f3340 ldr      r3, [pc, #0x340] ; [00046718] = 9d801000 
000463d4: e5832000 str      r2, [r3]
000463d8: e59f0374 ldr      r0, [pc, #0x374] ; [00046754] = 00054bc4 
000463dc: ebfffc96 bl       #0x4563c
000463e0: ea000009 b        #0x4640c
000463e4: e3520001 cmp      r2, #1
000463e8: 1a000005 bne      #0x46404
000463ec: e59f2364 ldr      r2, [pc, #0x364] ; [00046758] = 000055aa 
000463f0: e59f3320 ldr      r3, [pc, #0x320] ; [00046718] = 9d801000 
000463f4: e5832000 str      r2, [r3]
000463f8: e59f035c ldr      r0, [pc, #0x35c] ; [0004675c] = 00054bd4 
000463fc: ebfffc8e bl       #0x4563c
00046400: ea000001 b        #0x4640c
00046404: e59f0354 ldr      r0, [pc, #0x354] ; [00046760] = 00054be4 
00046408: ebfffc8b bl       #0x4563c
0004640c: e59d3094 ldr      r3, [sp, #0x94]
00046410: e3530000 cmp      r3, #0
00046414: 0a000004 beq      #0x4642c
00046418: e59f2338 ldr      r2, [pc, #0x338] ; [00046758] = 000055aa 
0004641c: e59f32f4 ldr      r3, [pc, #0x2f4] ; [00046718] = 9d801000 
00046420: e5832004 str      r2, [r3, #4]
00046424: e59f0338 ldr      r0, [pc, #0x338] ; [00046764] = 00054bf4 
00046428: ebfffc83 bl       #0x4563c
0004642c: e59d30a8 ldr      r3, [sp, #0xa8]
00046430: e3530000 cmp      r3, #0
00046434: 0a000012 beq      #0x46484
00046438: e59d20a4 ldr      r2, [sp, #0xa4]
0004643c: e1a03382 lsl      r3, r2, #7
00046440: e0433102 sub      r3, r3, r2, lsl #2
00046444: e0833002 add      r3, r3, r2
00046448: e1a04183 lsl      r4, r3, #3
0004644c: e59f0314 ldr      r0, [pc, #0x314] ; [00046768] = 00054c08 
00046450: e59d10ac ldr      r1, [sp, #0xac]
00046454: ebfffc78 bl       #0x4563c
00046458: e3540009 cmp      r4, #9
0004645c: da000008 ble      #0x46484
00046460: e28d0010 add      r0, sp, #0x10
00046464: e3a01008 mov      r1, #8
00046468: e3a0200a mov      r2, #0xa
0004646c: eb000685 bl       #0x47e88
00046470: e3700002 cmn      r0, #2
00046474: 0a0000a5 beq      #0x46710
00046478: e244400a sub      r4, r4, #0xa
0004647c: e3540009 cmp      r4, #9
00046480: cafffff6 bgt      #0x46460
00046484: e59f32b4 ldr      r3, [pc, #0x2b4] ; [00046740] = 00055010 
00046488: e5936000 ldr      r6, [r3]
0004648c: e5963000 ldr      r3, [r6]
00046490: e3130001 tst      r3, #1
00046494: 0a000001 beq      #0x464a0
00046498: e59f02cc ldr      r0, [pc, #0x2cc] ; [0004676c] = 00054c4c 
0004649c: ebfffc66 bl       #0x4563c
000464a0: e59f32c8 ldr      r3, [pc, #0x2c8] ; [00046770] = 00000050 
000464a4: e5933034 ldr      r3, [r3, #0x34]
000464a8: e3e00000 mvn      r0, #0
000464ac: e12fff33 blx      r3
000464b0: e1a07000 mov      r7, r0
000464b4: e3a04000 mov      r4, #0
000464b8: e59f52b0 ldr      r5, [pc, #0x2b0] ; [00046770] = 00000050 
000464bc: e5953034 ldr      r3, [r5, #0x34]
000464c0: e1a00004 mov      r0, r4
000464c4: e12fff33 blx      r3
000464c8: e5953014 ldr      r3, [r5, #0x14]
000464cc: e5932014 ldr      r2, [r3, #0x14]
000464d0: e5930000 ldr      r0, [r3]
000464d4: e3a01004 mov      r1, #4
000464d8: e12fff32 blx      r2
000464dc: e2844001 add      r4, r4, #1
000464e0: e3540002 cmp      r4, #2
000464e4: 1afffff3 bne      #0x464b8
000464e8: e5953034 ldr      r3, [r5, #0x34]
000464ec: e1a00007 mov      r0, r7
000464f0: e12fff33 blx      r3
000464f4: e5955014 ldr      r5, [r5, #0x14]
000464f8: e5953014 ldr      r3, [r5, #0x14]
000464fc: e5950000 ldr      r0, [r5]
00046500: e3a01009 mov      r1, #9
00046504: e12fff33 blx      r3
00046508: e59d3090 ldr      r3, [sp, #0x90]
0004650c: e3530000 cmp      r3, #0
00046510: 0a000000 beq      #0x46518
00046514: ebffe649 bl       #0x3fe40
00046518: e10f8000 mrs      r8, apsr
0004651c: e10f4000 mrs      r4, apsr
00046520: e38440c0 orr      r4, r4, #0xc0
00046524: e129f004 msr      cpsr_fc, r4
00046528: e5963000 ldr      r3, [r6]
0004652c: e3130001 tst      r3, #1
00046530: 0a000001 beq      #0x4653c
00046534: e59f0238 ldr      r0, [pc, #0x238] ; [00046774] = 00054c70 
00046538: ebfffc3f bl       #0x4563c
0004653c: ee17ff7a mrc      p15, #0, apsr_nzcv, c7, c10, #3
00046540: 1afffffd bne      #0x4653c
00046544: e3a07000 mov      r7, #0
00046548: ee077f9a mcr      p15, #0, r7, c7, c10, #4
0004654c: e59d30a0 ldr      r3, [sp, #0xa0]
00046550: e3530000 cmp      r3, #0
00046554: 1a000013 bne      #0x465a8
00046558: ee111f10 mrc      p15, #0, r1, c1, c0, #0
0004655c: e3c11a01 bic      r1, r1, #0x1000
00046560: ee011f10 mcr      p15, #0, r1, c1, c0, #0
00046564: e3a01000 mov      r1, #0
00046568: ee071f15 mcr      p15, #0, r1, c7, c5, #0
0004656c: e1a00000 mov      r0, r0
00046570: e1a00000 mov      r0, r0
00046574: e1a00000 mov      r0, r0
00046578: e1a00000 mov      r0, r0
0004657c: e1a00000 mov      r0, r0
00046580: e1a00000 mov      r0, r0
00046584: ee111f10 mrc      p15, #0, r1, c1, c0, #0
00046588: e3c11004 bic      r1, r1, #4
0004658c: ee011f10 mcr      p15, #0, r1, c1, c0, #0
00046590: e3a01000 mov      r1, #0
00046594: ee071f16 mcr      p15, #0, r1, c7, c6, #0
00046598: ee17ff7a mrc      p15, #0, apsr_nzcv, c7, c10, #3
0004659c: 1afffffd bne      #0x46598
000465a0: e3a07000 mov      r7, #0
000465a4: ee077f9a mcr      p15, #0, r7, c7, c10, #4
000465a8: e3a01000 mov      r1, #0
000465ac: ee071f15 mcr      p15, #0, r1, c7, c5, #0
000465b0: ee081f15 mcr      p15, #0, r1, c8, c5, #0
000465b4: e1a00000 mov      r0, r0
000465b8: e1a00000 mov      r0, r0
000465bc: e1a00000 mov      r0, r0
000465c0: e1a00000 mov      r0, r0
000465c4: e1a00000 mov      r0, r0
000465c8: e1a00000 mov      r0, r0
000465cc: ee17ff7a mrc      p15, #0, apsr_nzcv, c7, c10, #3
000465d0: 1afffffd bne      #0x465cc
000465d4: e3a07000 mov      r7, #0
000465d8: ee077f9a mcr      p15, #0, r7, c7, c10, #4
000465dc: e3a00000 mov      r0, #0
000465e0: ee070f16 mcr      p15, #0, r0, c7, c6, #0
000465e4: ee080f17 mcr      p15, #0, r0, c8, c7, #0
000465e8: e5963000 ldr      r3, [r6]
000465ec: e3130001 tst      r3, #1
000465f0: 0a000004 beq      #0x46608
000465f4: e59f017c ldr      r0, [pc, #0x17c] ; [00046778] = 00054c90 
000465f8: e59f317c ldr      r3, [pc, #0x17c] ; [0004677c] = 00017484 
000465fc: e5931000 ldr      r1, [r3]
00046600: e59d20ac ldr      r2, [sp, #0xac]
00046604: ebfffc0c bl       #0x4563c
00046608: e59f316c ldr      r3, [pc, #0x16c] ; [0004677c] = 00017484 
0004660c: e5933000 ldr      r3, [r3]
00046610: e3c3200f bic      r2, r3, #0xf
00046614: e2421050 sub      r1, r2, #0x50
00046618: e3a03000 mov      r3, #0
0004661c: e7813103 str      r3, [r1, r3, lsl #2]
00046620: e2833001 add      r3, r3, #1
00046624: e353000b cmp      r3, #0xb
00046628: 1afffffb bne      #0x4661c
0004662c: e59d30ac ldr      r3, [sp, #0xac]
00046630: e5813000 str      r3, [r1]
00046634: e5812034 str      r2, [r1, #0x34]
00046638: e59f3140 ldr      r3, [pc, #0x140] ; [00046780] = 00045bdc 
0004663c: e5813038 str      r3, [r1, #0x38]
00046640: e581303c str      r3, [r1, #0x3c]
00046644: e3a03013 mov      r3, #0x13
00046648: e5813040 str      r3, [r1, #0x40]
0004664c: e59f3128 ldr      r3, [pc, #0x128] ; [0004677c] = 00017484 
00046650: e5831000 str      r1, [r3]
00046654: e5963000 ldr      r3, [r6]
00046658: e3130001 tst      r3, #1
0004665c: 0a000001 beq      #0x46668
00046660: e59f011c ldr      r0, [pc, #0x11c] ; [00046784] = 00054cc8 
00046664: ebfffbf4 bl       #0x4563c
00046668: e59f010c ldr      r0, [pc, #0x10c] ; [0004677c] = 00017484 
0004666c: e59f1114 ldr      r1, [pc, #0x114] ; [00046788] = 0000d53c 
00046670: eb001122 bl       #0x4ab00
00046674: e59d30a0 ldr      r3, [sp, #0xa0]
00046678: e3530000 cmp      r3, #0
0004667c: 1a000006 bne      #0x4669c
00046680: ee111f10 mrc      p15, #0, r1, c1, c0, #0
00046684: e3811a01 orr      r1, r1, #0x1000
00046688: e3811003 orr      r1, r1, #3
0004668c: ee011f10 mcr      p15, #0, r1, c1, c0, #0
00046690: ee111f10 mrc      p15, #0, r1, c1, c0, #0
00046694: e3811007 orr      r1, r1, #7
00046698: ee011f10 mcr      p15, #0, r1, c1, c0, #0
0004669c: e3a01000 mov      r1, #0
000466a0: ee071f15 mcr      p15, #0, r1, c7, c5, #0
000466a4: ee081f15 mcr      p15, #0, r1, c8, c5, #0
000466a8: e1a00000 mov      r0, r0
000466ac: e1a00000 mov      r0, r0
000466b0: e1a00000 mov      r0, r0
000466b4: e1a00000 mov      r0, r0
000466b8: e1a00000 mov      r0, r0
000466bc: e1a00000 mov      r0, r0
000466c0: ee17ff7a mrc      p15, #0, apsr_nzcv, c7, c10, #3
000466c4: 1afffffd bne      #0x466c0
000466c8: e3a07000 mov      r7, #0
000466cc: ee077f9a mcr      p15, #0, r7, c7, c10, #4
000466d0: e3a00000 mov      r0, #0
000466d4: ee070f16 mcr      p15, #0, r0, c7, c6, #0
000466d8: ee080f17 mcr      p15, #0, r0, c8, c7, #0
000466dc: e5953014 ldr      r3, [r5, #0x14]
000466e0: e5950000 ldr      r0, [r5]
000466e4: e3a0100a mov      r1, #0xa
000466e8: e12fff33 blx      r3
000466ec: e10f3000 mrs      r3, apsr
000466f0: e20840c0 and      r4, r8, #0xc0
000466f4: e3c330c0 bic      r3, r3, #0xc0
000466f8: e1833004 orr      r3, r3, r4
000466fc: e129f003 msr      cpsr_fc, r3
00046700: e59f0084 ldr      r0, [pc, #0x84] ; [0004678c] = 00054cf8 
00046704: e59f3084 ldr      r3, [pc, #0x84] ; [00046790] = 0000d540 
00046708: e5931000 ldr      r1, [r3]
0004670c: ebfffbca bl       #0x4563c
00046710: e28dd0b0 add      sp, sp, #0xb0
00046714: e8bd81f0 pop      {r4, r5, r6, r7, r8, pc}
000467f4: e92d4030 push     {r4, r5, lr}
000467f8: e24dd02c sub      sp, sp, #0x2c
000467fc: e59f0090 ldr      r0, [pc, #0x90] ; [00046894] = 00054d50 
00046800: ebfffb8d bl       #0x4563c
00046804: e3a05001 mov      r5, #1
00046808: e59f3088 ldr      r3, [pc, #0x88] ; [00046898] = 00017214 
0004680c: e5835000 str      r5, [r3]
00046810: e59f0084 ldr      r0, [pc, #0x84] ; [0004689c] = 00054d60 '/Rom/image/8000_mmi.rap'
00046814: e59f1084 ldr      r1, [pc, #0x84] ; [000468a0] = 00a00000 
00046818: ebffc0f5 bl       #0x36bf4
0004681c: e2504000 subs     r4, r0, #0
00046820: 0a000010 beq      #0x46868
00046824: e59f3078 ldr      r3, [pc, #0x78] ; [000468a4] = 00054d1c 
00046828: e58d3020 str      r3, [sp, #0x20]
0004682c: e59f3074 ldr      r3, [pc, #0x74] ; [000468a8] = 00054d20 
00046830: e58d3024 str      r3, [sp, #0x24]
00046834: e1a0000d mov      r0, sp
00046838: e58dd028 str      sp, [sp, #0x28]
0004683c: e59f1068 ldr      r1, [pc, #0x68] ; [000468ac] = 00054d24 '0x%x'
00046840: e1a02004 mov      r2, r4
00046844: ebfffc11 bl       #0x45890
00046848: e59f0060 ldr      r0, [pc, #0x60] ; [000468b0] = 00054d78 
0004684c: e1a01004 mov      r1, r4
00046850: ebfffb79 bl       #0x4563c
00046854: e3a00003 mov      r0, #3
00046858: e28d1020 add      r1, sp, #0x20
0004685c: ebfffe6f bl       #0x46220
00046860: e1a04005 mov      r4, r5
00046864: ea000005 b        #0x46880
00046868: e59f0044 ldr      r0, [pc, #0x44] ; [000468b4] = 00054d9c 
0004686c: ebfffb72 bl       #0x4563c
00046870: e3a02000 mov      r2, #0
00046874: e59f301c ldr      r3, [pc, #0x1c] ; [00046898] = 00017214 
00046878: e5832000 str      r2, [r3]
0004687c: e1a04002 mov      r4, r2
00046880: e59f0030 ldr      r0, [pc, #0x30] ; [000468b8] = 00054dc4 
00046884: ebfffb6c bl       #0x4563c
00046888: e1a00004 mov      r0, r4
0004688c: e28dd02c add      sp, sp, #0x2c
00046890: e8bd8030 pop      {r4, r5, pc}
00046894: 00054d50 andeq    r4, r5, r0, asr sp
00046898: 00017214 andeq    r7, r1, r4, lsl r2
0004689c: 00054d60 andeq    r4, r5, r0, ror #26
000468a0: 00a00000 adceq    r0, r0, r0
000468a4: 00054d1c andeq    r4, r5, ip, lsl sp
000468a8: 00054d20 andeq    r4, r5, r0, lsr #26
000468ac: 00054d24 andeq    r4, r5, r4, lsr #26
000468b0: 00054d78 andeq    r4, r5, r8, ror sp
000468b4: 00054d9c muleq    r5, ip, sp
000468b8: 00054dc4 andeq    r4, r5, r4, asr #27
00046ad4: e59f243c ldr      r2, [pc, #0x43c] ; [00046f18] = 00017214 
00046ad8: e5926000 ldr      r6, [r2]
00046adc: e3560000 cmp      r6, #0
00046ae0: 1a00003e bne      #0x46be0
00046ae4: e59f3430 ldr      r3, [pc, #0x430] ; [00046f1c] = 000171f4 
00046ae8: e5933000 ldr      r3, [r3]
00046aec: e3530000 cmp      r3, #0
00046af0: 0a0000f2 beq      #0x46ec0
00046af4: e3a03001 mov      r3, #1
00046af8: e5823000 str      r3, [r2]
00046afc: e59f341c ldr      r3, [pc, #0x41c] ; [00046f20] = 000171e8 
00046b00: e5933000 ldr      r3, [r3]
00046b04: e3530000 cmp      r3, #0
00046b08: 0a000015 beq      #0x46b64
00046b0c: e59f3410 ldr      r3, [pc, #0x410] ; [00046f24] = 000171f0 
00046b10: e5933000 ldr      r3, [r3]
00046b14: e3530000 cmp      r3, #0
00046b18: 0a000003 beq      #0x46b2c
00046b1c: e59f0404 ldr      r0, [pc, #0x404] ; [00046f28] = 00054d60 '/Rom/image/8000_mmi.rap'
00046b20: ebffbfe3 bl       #0x36ab4
00046b24: e1a04000 mov      r4, r0
00046b28: ea000010 b        #0x46b70
00046b2c: e59f33f8 ldr      r3, [pc, #0x3f8] ; [00046f2c] = 000174dc 
00046b30: e5933000 ldr      r3, [r3]
00046b34: e3530000 cmp      r3, #0
00046b38: 0a000004 beq      #0x46b50
00046b3c: e59f03ec ldr      r0, [pc, #0x3ec] ; [00046f30] = 00054dd4 '/Rom/image/linux_kernel.rap'
00046b40: e59f13ec ldr      r1, [pc, #0x3ec] ; [00046f34] = 00a00000 
00046b44: ebffc02a bl       #0x36bf4
00046b48: e1a04000 mov      r4, r0
00046b4c: ea000007 b        #0x46b70
00046b50: e59f03d0 ldr      r0, [pc, #0x3d0] ; [00046f28] = 00054d60 '/Rom/image/8000_mmi.rap'
00046b54: e59f13d8 ldr      r1, [pc, #0x3d8] ; [00046f34] = 00a00000 
00046b58: ebffc025 bl       #0x36bf4
00046b5c: e1a04000 mov      r4, r0
00046b60: ea000002 b        #0x46b70
00046b64: e59f03c8 ldr      r0, [pc, #0x3c8] ; [00046f34] = 00a00000 
00046b68: ebffdf5e bl       #0x3e8e8
00046b6c: e1a04000 mov      r4, r0
00046b70: e3540000 cmp      r4, #0
00046b74: 0a000013 beq      #0x46bc8
00046b78: e59f03b8 ldr      r0, [pc, #0x3b8] ; [00046f38] = 00054df0 
00046b7c: e1a01004 mov      r1, r4
00046b80: ebfffaad bl       #0x4563c
00046b84: e59f33a0 ldr      r3, [pc, #0x3a0] ; [00046f2c] = 000174dc 
00046b88: e5933000 ldr      r3, [r3]
00046b8c: e3530000 cmp      r3, #0
00046b90: 0a000004 beq      #0x46ba8
00046b94: e59f03a0 ldr      r0, [pc, #0x3a0] ; [00046f3c] = 00017440 
00046b98: e59f13a0 ldr      r1, [pc, #0x3a0] ; [00046f40] = 00054e14 
00046b9c: e1a02004 mov      r2, r4
00046ba0: ebfffb3a bl       #0x45890
00046ba4: ea000003 b        #0x46bb8
00046ba8: e59f038c ldr      r0, [pc, #0x38c] ; [00046f3c] = 00017440 
00046bac: e59f1390 ldr      r1, [pc, #0x390] ; [00046f44] = 00054e20 
00046bb0: e1a02004 mov      r2, r4
00046bb4: ebfffb35 bl       #0x45890
00046bb8: e59f237c ldr      r2, [pc, #0x37c] ; [00046f3c] = 00017440 
00046bbc: e59f3354 ldr      r3, [pc, #0x354] ; [00046f18] = 00017214 
00046bc0: e5832000 str      r2, [r3]
00046bc4: ea000001 b        #0x46bd0
00046bc8: e59f0378 ldr      r0, [pc, #0x378] ; [00046f48] = 00054da8 
00046bcc: ebfffa9a bl       #0x4563c
0004ab00: e24dc014 sub      ip, sp, #0x14
0004ab04: e92c6000 stmdb    ip!, {sp, lr}
0004ab08: e1a0d00c mov      sp, ip
0004ab0c: e92d1fff push     {r0, r1, r2, r3, r4, r5, r6, r7, r8, sb, sl, fp, ip}
0004ab10: e10f2000 mrs      r2, apsr
0004ab14: e58d2040 str      r2, [sp, #0x40]
0004ab18: e581d000 str      sp, [r1]
0004ab1c: e590b000 ldr      fp, [r0]
0004ab20: e10f0000 mrs      r0, apsr
0004ab24: e38000c0 orr      r0, r0, #0xc0
0004ab28: e129f000 msr      cpsr_fc, r0
0004ab2c: e59b0040 ldr      r0, [fp, #0x40]
0004ab30: e169f000 msr      spsr_fc, r0
0004ab34: e89b7fff ldm      fp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, sb, sl, fp, ip, sp, lr}
0004ab38: e1b0f00e movs     pc, lr
