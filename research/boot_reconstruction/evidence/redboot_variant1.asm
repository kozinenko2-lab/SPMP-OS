00038f0c: e1a0c00d mov      ip, sp
00038f10: e92dd9f0 push     {r4, r5, r6, r7, r8, fp, ip, lr, pc}
00038f14: e24cb004 sub      fp, ip, #4
00038f18: e1a06000 mov      r6, r0
00038f1c: e1a07001 mov      r7, r1
00038f20: e1a04001 mov      r4, r1
00038f24: e3a08000 mov      r8, #0
00038f28: e59f0070 ldr      r0, [pc, #0x70] ; [00038fa0] = 000561fc 
00038f2c: eb0043c6 bl       #0x49e4c
00038f30: e1a00006 mov      r0, r6
00038f34: e3a01001 mov      r1, #1
00038f38: eb00299c bl       #0x435b0
00038f3c: e2505000 subs     r5, r0, #0
00038f40: ba00000a blt      #0x38f70
00038f44: e1a00005 mov      r0, r5
00038f48: e1a01004 mov      r1, r4
00038f4c: e3a02a01 mov      r2, #0x1000
00038f50: eb002bae bl       #0x43e10
00038f54: e2844a01 add      r4, r4, #0x1000
00038f58: e3500000 cmp      r0, #0
00038f5c: 1afffff8 bne      #0x38f44
00038f60: e1a00005 mov      r0, r5
00038f64: eb002bb5 bl       #0x43e40
00038f68: e3a03000 mov      r3, #0
00038f6c: ea000003 b        #0x38f80
00038f70: e59f002c ldr      r0, [pc, #0x2c] ; [00038fa4] = 00056214 
00038f74: e1a01006 mov      r1, r6
00038f78: eb0043b3 bl       #0x49e4c
00038f7c: e3e03000 mvn      r3, #0
00038f80: e3530000 cmp      r3, #0
00038f84: ba000003 blt      #0x38f98
00038f88: e1a00007 mov      r0, r7
00038f8c: e3a01001 mov      r1, #1
00038f90: eb002604 bl       #0x427a8
00038f94: e1a08000 mov      r8, r0
00038f98: e1a00008 mov      r0, r8
00038f9c: e89da9f0 ldm      sp, {r4, r5, r6, r7, r8, fp, sp, pc}
00038fa0: 000561fc strdeq   r6, r7, [r5], -ip
00038fa4: 00056214 andeq    r6, r5, r4, lsl r2
0004a994: e1a0c00d mov      ip, sp
0004a998: e92dd870 push     {r4, r5, r6, fp, ip, lr, pc}
0004a99c: e24cb004 sub      fp, ip, #4
0004a9a0: e24dd02c sub      sp, sp, #0x2c
0004a9a4: e3a05000 mov      r5, #0
0004a9a8: e59f008c ldr      r0, [pc, #0x8c] ; [0004aa3c] = 00059ba4 
0004a9ac: ebfffd26 bl       #0x49e4c
0004a9b0: e3a06001 mov      r6, #1
0004a9b4: e59f3084 ldr      r3, [pc, #0x84] ; [0004aa40] = 0001303c 
0004a9b8: e5836000 str      r6, [r3]
0004a9bc: e59f0080 ldr      r0, [pc, #0x80] ; [0004aa44] = 00059bb4 '/Rom/image/8000_mmi.rap'
0004a9c0: e59f1080 ldr      r1, [pc, #0x80] ; [0004aa48] = 00a00000 
0004a9c4: ebffb950 bl       #0x38f0c
0004a9c8: e2504000 subs     r4, r0, #0
0004a9cc: 0a000010 beq      #0x4aa14
0004a9d0: e59f3074 ldr      r3, [pc, #0x74] ; [0004aa4c] = 00059828 
0004a9d4: e50b3044 str      r3, [fp, #-0x44]
0004a9d8: e59f3070 ldr      r3, [pc, #0x70] ; [0004aa50] = 00059bcc 
0004a9dc: e50b3040 str      r3, [fp, #-0x40]
0004a9e0: e24b0038 sub      r0, fp, #0x38
0004a9e4: e50b003c str      r0, [fp, #-0x3c]
0004a9e8: e59f1064 ldr      r1, [pc, #0x64] ; [0004aa54] = 00059bd0 '0x%x'
0004a9ec: e1a02004 mov      r2, r4
0004a9f0: ebfffcb7 bl       #0x49cd4
0004a9f4: e59f005c ldr      r0, [pc, #0x5c] ; [0004aa58] = 00059bd8 
0004a9f8: e1a01004 mov      r1, r4
0004a9fc: ebfffd12 bl       #0x49e4c
0004aa00: e3a00003 mov      r0, #3
0004aa04: e24b1044 sub      r1, fp, #0x44
0004aa08: eb00023d bl       #0x4b304
0004aa0c: e1a05006 mov      r5, r6
0004aa10: ea000004 b        #0x4aa28
0004aa14: e59f0040 ldr      r0, [pc, #0x40] ; [0004aa5c] = 00059bfc 
0004aa18: ebfffd0b bl       #0x49e4c
0004aa1c: e3a02000 mov      r2, #0
0004aa20: e59f3018 ldr      r3, [pc, #0x18] ; [0004aa40] = 0001303c 
0004aa24: e5832000 str      r2, [r3]
0004aa28: e59f0030 ldr      r0, [pc, #0x30] ; [0004aa60] = 00059c24 
0004aa2c: ebfffd06 bl       #0x49e4c
0004aa30: e1a00005 mov      r0, r5
0004aa34: e24bd018 sub      sp, fp, #0x18
0004aa38: e89da870 ldm      sp, {r4, r5, r6, fp, sp, pc}
0004aa3c: 00059ba4 andeq    sb, r5, r4, lsr #23
0004aa40: 0001303c andeq    r3, r1, ip, lsr r0
0004aa44: 00059bb4 strheq   sb, [r5], -r4
0004aa48: 00a00000 adceq    r0, r0, r0
0004aa4c: 00059828 andeq    sb, r5, r8, lsr #16
0004aa50: 00059bcc andeq    sb, r5, ip, asr #23
0004aa54: 00059bd0 ldrdeq   sb, sl, [r5], -r0
0004aa58: 00059bd8 ldrdeq   sb, sl, [r5], -r8
0004aa5c: 00059bfc strdeq   sb, sl, [r5], -ip
0004aa60: 00059c24 andeq    sb, r5, r4, lsr #24
