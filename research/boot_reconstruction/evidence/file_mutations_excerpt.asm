; Selected original ARM instructions, not buildable source.
; 8000_MMI.RAP SHA256 5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e
; Wrappers and rename dispatch; driver no-replace semantics remain unverified.
00283858: e1a0c00d mov      ip, sp
0028385c: e92dd800 push     {fp, ip, lr, pc}
00283860: e24cb004 sub      fp, ip, #4
00283864: e24dd004 sub      sp, sp, #4
00283868: e24b000c sub      r0, fp, #0xc
0028386c: e59f3068 ldr      r3, [pc, #0x68] ; [002838dc] = 005704c8
00283870: e5203004 str      r3, [r0, #-4]!
00283874: e1a01002 mov      r1, r2
00283878: ebffff17 bl       #0x2834dc
0028387c: e51b0010 ldr      r0, [fp, #-0x10]
00283880: eb009852 bl       #0x2a99d0
00283884: e3500000 cmp      r0, #0
00283888: a3a00000 movge    r0, #0
0028388c: a99da800 ldmibge  sp, {fp, sp, pc}
00283890: eb009e36 bl       #0x2ab170
00283894: e3700008 cmn      r0, #8
00283898: 13700011 cmnne    r0, #0x11
0028389c: 0a000007 beq      #0x2838c0
002838a0: eb009e32 bl       #0x2ab170
002838a4: e1a03000 mov      r3, r0
002838a8: e59f0030 ldr      r0, [pc, #0x30] ; [002838e0] = 004d00c0
002838ac: e59f1030 ldr      r1, [pc, #0x30] ; [002838e4] = 004d3ba4 'fatfs_unlink'
002838b0: e3a02f93 mov      r2, #0x24c
002838b4: eb0054aa bl       #0x298b64
002838b8: e3a00005 mov      r0, #5
002838bc: e99da800 ldmib    sp, {fp, sp, pc}
002838c0: e3700011 cmn      r0, #0x11
002838c4: 03a000c8 moveq    r0, #0xc8
002838c8: 099da800 ldmibeq  sp, {fp, sp, pc}
002838cc: e3700008 cmn      r0, #8
002838d0: 03a00015 moveq    r0, #0x15
002838d4: 13a00016 movne    r0, #0x16
002838d8: e99da800 ldmib    sp, {fp, sp, pc}
002838dc: 005704c8 subseq   r0, r7, r8, asr #9
002838e0: 004d00c0 subeq    r0, sp, r0, asr #1
002838e4: 004d3ba4 subeq    r3, sp, r4, lsr #23
002838e8: e1a0c00d mov      ip, sp
002838ec: e92dd810 push     {r4, fp, ip, lr, pc}
002838f0: e24cb004 sub      fp, ip, #4
002838f4: e24dd004 sub      sp, sp, #4
002838f8: e24b0010 sub      r0, fp, #0x10
002838fc: e59f3090 ldr      r3, [pc, #0x90] ; [00283994] = 005704c8
00283900: e5203004 str      r3, [r0, #-4]!
00283904: e1a01002 mov      r1, r2
00283908: ebfffef3 bl       #0x2834dc
0028390c: e51b0014 ldr      r0, [fp, #-0x14]
00283910: e3a01000 mov      r1, #0
00283914: eb009161 bl       #0x2a7ea0
00283918: e3500000 cmp      r0, #0
0028391c: a3a00000 movge    r0, #0
00283920: a99da810 ldmibge  sp, {r4, fp, sp, pc}
00283924: eb009e11 bl       #0x2ab170
00283928: e1a04000 mov      r4, r0
0028392c: e3700007 cmn      r0, #7
00283930: 13700014 cmnne    r0, #0x14
00283934: 0a00000e beq      #0x283974
00283938: e3e03d19 mvn      r3, #0x640
0028393c: e2433001 sub      r3, r3, #1
00283940: e1500003 cmp      r0, r3
00283944: 0a00000a beq      #0x283974
00283948: e59f0048 ldr      r0, [pc, #0x48] ; [00283998] = 004d0238 '[FS]::%s(%d):'
0028394c: e59f1048 ldr      r1, [pc, #0x48] ; [0028399c] = 004d3bd8 'fatfs_mkdir'
00283950: e3a02f9f mov      r2, #0x27c
00283954: e2822002 add      r2, r2, #2
00283958: eb005481 bl       #0x298b64
0028395c: e59f003c ldr      r0, [pc, #0x3c] ; [002839a0] = 004d0248
00283960: e1a01004 mov      r1, r4
00283964: e51b2014 ldr      r2, [fp, #-0x14]
00283968: eb00547d bl       #0x298b64
0028396c: e3a00005 mov      r0, #5
00283970: e99da810 ldmib    sp, {r4, fp, sp, pc}
00283974: e3e03d19 mvn      r3, #0x640
00283978: e2433001 sub      r3, r3, #1
0028397c: e3740014 cmn      r4, #0x14
00283980: 11540003 cmpne    r4, r3
00283984: 03a00f4b moveq    r0, #0x12c
00283988: 02800002 addeq    r0, r0, #2
0028398c: 13a00014 movne    r0, #0x14
00283990: e99da810 ldmib    sp, {r4, fp, sp, pc}
00283994: 005704c8 subseq   r0, r7, r8, asr #9
00283998: 004d0238 subeq    r0, sp, r8, lsr r2
0028399c: 004d3bd8 ldrdeq   r3, r4, [sp], #-0xb8
002839a0: 004d0248 subeq    r0, sp, r8, asr #4
002839a4: e1a0c00d mov      ip, sp
002839a8: e92dd800 push     {fp, ip, lr, pc}
002839ac: e24cb004 sub      fp, ip, #4
002839b0: e24dd004 sub      sp, sp, #4
002839b4: e24b000c sub      r0, fp, #0xc
002839b8: e59f3068 ldr      r3, [pc, #0x68] ; [00283a28] = 005704c8
002839bc: e5203004 str      r3, [r0, #-4]!
002839c0: e1a01002 mov      r1, r2
002839c4: ebfffec4 bl       #0x2834dc
002839c8: e51b0010 ldr      r0, [fp, #-0x10]
002839cc: eb009095 bl       #0x2a7c28
002839d0: e3500000 cmp      r0, #0
002839d4: a3a00000 movge    r0, #0
002839d8: a99da800 ldmibge  sp, {fp, sp, pc}
002839dc: eb009de3 bl       #0x2ab170
002839e0: e3e03d19 mvn      r3, #0x640
002839e4: e2433008 sub      r3, r3, #8
002839e8: e1500003 cmp      r0, r3
002839ec: 13700007 cmnne    r0, #7
002839f0: 0a000008 beq      #0x283a18
002839f4: eb009ddd bl       #0x2ab170
002839f8: e1a03000 mov      r3, r0
002839fc: e59f0028 ldr      r0, [pc, #0x28] ; [00283a2c] = 004d00c0
00283a00: e59f1028 ldr      r1, [pc, #0x28] ; [00283a30] = 004d3c0c 'fatfs_rmdir'
00283a04: e3a02faa mov      r2, #0x2a8
00283a08: e2822001 add      r2, r2, #1
00283a0c: eb005454 bl       #0x298b64
00283a10: e3a00005 mov      r0, #5
00283a14: e99da800 ldmib    sp, {fp, sp, pc}
00283a18: e3700007 cmn      r0, #7
00283a1c: 13a00016 movne    r0, #0x16
00283a20: 03a00014 moveq    r0, #0x14
00283a24: e99da800 ldmib    sp, {fp, sp, pc}
00283a28: 005704c8 subseq   r0, r7, r8, asr #9
00283a2c: 004d00c0 subeq    r0, sp, r0, asr #1
00283a30: 004d3c0c subeq    r3, sp, ip, lsl #24
00283a34: e1a0c00d mov      ip, sp
00283a38: e92dd810 push     {r4, fp, ip, lr, pc}
00283a3c: e24cb004 sub      fp, ip, #4
00283a40: e24dd008 sub      sp, sp, #8
00283a44: e59f30ac ldr      r3, [pc, #0xac] ; [00283af8] = 005704c8
00283a48: e50b3014 str      r3, [fp, #-0x14]
00283a4c: e59f30a8 ldr      r3, [pc, #0xa8] ; [00283afc] = 005705d0
00283a50: e50b3018 str      r3, [fp, #-0x18]
00283a54: e24b0014 sub      r0, fp, #0x14
00283a58: e1a01002 mov      r1, r2
00283a5c: ebfffe9e bl       #0x2834dc
00283a60: e24b0018 sub      r0, fp, #0x18
00283a64: e59b1004 ldr      r1, [fp, #4]
00283a68: ebfffe9b bl       #0x2834dc
00283a6c: e51b0014 ldr      r0, [fp, #-0x14]
00283a70: e51b1018 ldr      r1, [fp, #-0x18]
00283a74: eb009f1b bl       #0x2ab6e8
00283a78: e3500000 cmp      r0, #0
00283a7c: a3a00000 movge    r0, #0
00283a80: aa00001a bge      #0x283af0
00283a84: eb009db9 bl       #0x2ab170
00283a88: e1a04000 mov      r4, r0
00283a8c: e3700011 cmn      r0, #0x11
00283a90: 13700014 cmnne    r0, #0x14
00283a94: 0a000008 beq      #0x283abc
00283a98: eb009db4 bl       #0x2ab170
00283a9c: e1a03000 mov      r3, r0
00283aa0: e59f0058 ldr      r0, [pc, #0x58] ; [00283b00] = 004d00c0
00283aa4: e59f1058 ldr      r1, [pc, #0x58] ; [00283b04] = 004d3c34 'fatfs_rename'
00283aa8: e3a02fb6 mov      r2, #0x2d8
00283aac: e2822001 add      r2, r2, #1
00283ab0: eb00542b bl       #0x298b64
00283ab4: e1a00004 mov      r0, r4
00283ab8: ea00000c b        #0x283af0
00283abc: e3700011 cmn      r0, #0x11
00283ac0: 03a000c8 moveq    r0, #0xc8
00283ac4: 0a000009 beq      #0x283af0
00283ac8: ca000004 bgt      #0x283ae0
00283acc: e3740014 cmn      r4, #0x14
00283ad0: 03a00f4b moveq    r0, #0x12c
00283ad4: 02800002 addeq    r0, r0, #2
00283ad8: 0a000004 beq      #0x283af0
00283adc: ea000002 b        #0x283aec
00283ae0: e3740010 cmn      r4, #0x10
00283ae4: 03a0005f moveq    r0, #0x5f
00283ae8: 0a000000 beq      #0x283af0
00283aec: e3a00005 mov      r0, #5
00283af0: e24bd010 sub      sp, fp, #0x10
00283af4: e89da810 ldm      sp, {r4, fp, sp, pc}
002ab6e8: e1a0c00d mov      ip, sp
002ab6ec: e92dddf0 push     {r4, r5, r6, r7, r8, sl, fp, ip, lr, pc}
002ab6f0: e24cb004 sub      fp, ip, #4
002ab6f4: e24dd010 sub      sp, sp, #0x10
002ab6f8: e1a05000 mov      r5, r0
002ab6fc: e1a04001 mov      r4, r1
002ab700: e50b0028 str      r0, [fp, #-0x28]
002ab704: e50b1030 str      r1, [fp, #-0x30]
002ab708: e3e0a000 mvn      sl, #0
002ab70c: e59f0198 ldr      r0, [pc, #0x198] ; [002ab8ac] = 005dd824
002ab710: eb023a4c bl       #0x33a048
002ab714: e59f6194 ldr      r6, [pc, #0x194] ; [002ab8b0] = 005ddc48
002ab718: e3a02d7d mov      r2, #0x1f40
002ab71c: e2827008 add      r7, r2, #8
002ab720: e3a03000 mov      r3, #0
002ab724: e7863007 str      r3, [r6, r7]
002ab728: e282800c add      r8, r2, #0xc
002ab72c: e7c63008 strb     r3, [r6, r8]
002ab730: e1540003 cmp      r4, r3
002ab734: 11550003 cmpne    r5, r3
002ab738: 1a000016 bne      #0x2ab798
002ab73c: e59f3170 ldr      r3, [pc, #0x170] ; [002ab8b4] = 00515854
002ab740: e5935000 ldr      r5, [r3]
002ab744: e1a00005 mov      r0, r5
002ab748: eb026a6d bl       #0x346104
002ab74c: e1a04000 mov      r4, r0
002ab750: e3e03008 mvn      r3, #8
002ab754: e7863007 str      r3, [r6, r7]
002ab758: e2860d7d add      r0, r6, #0x1f40
002ab75c: e280000c add      r0, r0, #0xc
002ab760: e1a01005 mov      r1, r5
002ab764: e1a02004 mov      r2, r4
002ab768: ebffb53f bl       #0x298c6c
002ab76c: e0842006 add      r2, r4, r6
002ab770: e3a0303a mov      r3, #0x3a
002ab774: e7c23008 strb     r3, [r2, r8]
002ab778: e2861d7d add      r1, r6, #0x1f40
002ab77c: e281100d add      r1, r1, #0xd
002ab780: e3a00e6f mov      r0, #0x6f0
002ab784: e280000e add      r0, r0, #0xe
002ab788: e0841001 add      r1, r4, r1
002ab78c: e3a0200a mov      r2, #0xa
002ab790: eb0018a9 bl       #0x2b1a3c
002ab794: ea00003f b        #0x2ab898
002ab798: e24b0028 sub      r0, fp, #0x28
002ab79c: e24b102c sub      r1, fp, #0x2c
002ab7a0: eb0001d5 bl       #0x2abefc
002ab7a4: e3500000 cmp      r0, #0
002ab7a8: ba00003a blt      #0x2ab898
002ab7ac: e59f0104 ldr      r0, [pc, #0x104] ; [002ab8b8] = 005dec48
002ab7b0: e51b1028 ldr      r1, [fp, #-0x28]
002ab7b4: eb026a27 bl       #0x346058
002ab7b8: e59f30f8 ldr      r3, [pc, #0xf8] ; [002ab8b8] = 005dec48
002ab7bc: e50b3028 str      r3, [fp, #-0x28]
002ab7c0: e24b0030 sub      r0, fp, #0x30
002ab7c4: e24b1034 sub      r1, fp, #0x34
002ab7c8: eb0001cb bl       #0x2abefc
002ab7cc: e3500000 cmp      r0, #0
002ab7d0: ba000030 blt      #0x2ab898
002ab7d4: e51b202c ldr      r2, [fp, #-0x2c]
002ab7d8: e51b3034 ldr      r3, [fp, #-0x34]
002ab7dc: e1520003 cmp      r2, r3
002ab7e0: 1a00002c bne      #0x2ab898
002ab7e4: e5923034 ldr      r3, [r2, #0x34]
002ab7e8: e3530000 cmp      r3, #0
002ab7ec: 0a000002 beq      #0x2ab7fc
002ab7f0: e593306c ldr      r3, [r3, #0x6c]
002ab7f4: e3530000 cmp      r3, #0
002ab7f8: 1a00001d bne      #0x2ab874
002ab7fc: e59f60ac ldr      r6, [pc, #0xac] ; [002ab8b0] = 005ddc48
002ab800: e3a08d7d mov      r8, #0x1f40
002ab804: e2887008 add      r7, r8, #8
002ab808: e7963007 ldr      r3, [r6, r7]
002ab80c: e3530000 cmp      r3, #0
002ab810: 1a000020 bne      #0x2ab898
002ab814: e59f3098 ldr      r3, [pc, #0x98] ; [002ab8b4] = 00515854
002ab818: e5935000 ldr      r5, [r3]
002ab81c: e1a00005 mov      r0, r5
002ab820: eb026a37 bl       #0x346104
002ab824: e1a04000 mov      r4, r0
002ab828: e3e03008 mvn      r3, #8
002ab82c: e7863007 str      r3, [r6, r7]
002ab830: e0860008 add      r0, r6, r8
002ab834: e280000c add      r0, r0, #0xc
002ab838: e1a01005 mov      r1, r5
002ab83c: e1a02004 mov      r2, r4
002ab840: ebffb509 bl       #0x298c6c
002ab844: e288100c add      r1, r8, #0xc
002ab848: e0842006 add      r2, r4, r6
002ab84c: e3a0303a mov      r3, #0x3a
002ab850: e7c23001 strb     r3, [r2, r1]
002ab854: e0861008 add      r1, r6, r8
002ab858: e281100d add      r1, r1, #0xd
002ab85c: e3a00e71 mov      r0, #0x710
002ab860: e2800008 add      r0, r0, #8
002ab864: e0841001 add      r1, r4, r1
002ab868: e3a0200a mov      r2, #0xa
002ab86c: eb001872 bl       #0x2b1a3c
002ab870: ea000008 b        #0x2ab898
002ab874: e51b302c ldr      r3, [fp, #-0x2c]
002ab878: e2830030 add      r0, r3, #0x30
002ab87c: e8901001 ldm      r0, {r0, ip}
002ab880: e51b1028 ldr      r1, [fp, #-0x28]
002ab884: e51b2030 ldr      r2, [fp, #-0x30]
002ab888: e1a0e00f mov      lr, pc
002ab88c: e59cf06c ldr      pc, [ip, #0x6c]
002ab890: e3500000 cmp      r0, #0
002ab894: a3a0a000 movge    sl, #0
002ab898: e59f000c ldr      r0, [pc, #0xc] ; [002ab8ac] = 005dd824
002ab89c: eb0239ee bl       #0x33a05c
002ab8a0: e1a0000a mov      r0, sl
002ab8a4: e24bd024 sub      sp, fp, #0x24
002ab8a8: e89dadf0 ldm      sp, {r4, r5, r6, r7, r8, sl, fp, sp, pc}
00284100: e3a00000 mov r0, #0
00284104: e12fff1e bx lr
