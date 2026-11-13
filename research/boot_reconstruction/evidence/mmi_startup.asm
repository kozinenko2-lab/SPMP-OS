00280040: ee110f10 mrc      p15, #0, r0, c1, c0, #0
00280044: e3e01a02 mvn      r1, #0x2000
00280048: e0000001 and      r0, r0, r1
0028004c: ee010f10 mcr      p15, #0, r0, c1, c0, #0
00280050: e10f7000 mrs      r7, apsr
00280054: e207701f and      r7, r7, #0x1f
00280058: e3570013 cmp      r7, #0x13
0028005c: 0a000007 beq      #0x280080
00280060: e3a00000 mov      r0, #0
00280064: e59f1480 ldr      r1, [pc, #0x480] ; [002804ec] = 00280000 
00280068: e5912004 ldr      r2, [r1, #4]
0028006c: e5802004 str      r2, [r0, #4]
00280070: e5912024 ldr      r2, [r1, #0x24]
00280074: e5802024 str      r2, [r0, #0x24]
00280078: e5912008 ldr      r2, [r1, #8]
0028007c: e5802008 str      r2, [r0, #8]
00280080: e59f0438 ldr      r0, [pc, #0x438] ; [002804c0] = 005137e8 
00280084: e5901000 ldr      r1, [r0]
00280088: e3510000 cmp      r1, #0
0028008c: 1afffffd bne      #0x280088
00280090: e59f10c8 ldr      r1, [pc, #0xc8] ; [00280160] = deadb00b 
00280094: e5801000 str      r1, [r0]
00280098: e3a00000 mov      r0, #0
0028009c: e59f1418 ldr      r1, [pc, #0x418] ; [002804bc] = 00280000 
002800a0: e3570013 cmp      r7, #0x13
002800a4: 0a000001 beq      #0x2800b0
002800a8: e5912028 ldr      r2, [r1, #0x28]
002800ac: e5802028 str      r2, [r0, #0x28]
002800b0: e5912018 ldr      r2, [r1, #0x18]
002800b4: e5802018 str      r2, [r0, #0x18]
002800b8: e5912038 ldr      r2, [r1, #0x38]
002800bc: e5802038 str      r2, [r0, #0x38]
002800c0: e591201c ldr      r2, [r1, #0x1c]
002800c4: e580201c str      r2, [r0, #0x1c]
002800c8: e591203c ldr      r2, [r1, #0x3c]
002800cc: e580203c str      r2, [r0, #0x3c]
002800d0: e591200c ldr      r2, [r1, #0xc]
002800d4: e580200c str      r2, [r0, #0xc]
002800d8: e591202c ldr      r2, [r1, #0x2c]
002800dc: e580202c str      r2, [r0, #0x2c]
002800e0: e5912010 ldr      r2, [r1, #0x10]
002800e4: e5802010 str      r2, [r0, #0x10]
002800e8: e5912030 ldr      r2, [r1, #0x30]
002800ec: e5802030 str      r2, [r0, #0x30]
002800f0: e59fd394 ldr      sp, [pc, #0x394] ; [0028048c] = 0056c0a0 
002800f4: e3a000d2 mov      r0, #0xd2
002800f8: e129f000 msr      cpsr_fc, r0
002800fc: e59fd38c ldr      sp, [pc, #0x38c] ; [00280490] = 0056ae10 
00280100: e3a000db mov      r0, #0xdb
00280104: e129f000 msr      cpsr_fc, r0
00280108: e59fd380 ldr      sp, [pc, #0x380] ; [00280490] = 0056ae10 
0028010c: e3a000d3 mov      r0, #0xd3
00280110: e129f000 msr      cpsr_fc, r0
00280114: e169f000 msr      spsr_fc, r0
00280118: e59fd3ac ldr      sp, [pc, #0x3ac] ; [002804cc] = 0056be90 
0028011c: e59f1374 ldr      r1, [pc, #0x374] ; [00280498] = 0056ad90 
00280120: e59f2374 ldr      r2, [pc, #0x374] ; [0028049c] = 006c8fdc 
00280124: e3a00000 mov      r0, #0
00280128: e1510002 cmp      r1, r2
0028012c: 0a000002 beq      #0x28013c
00280130: e4810004 str      r0, [r1], #4
00280134: e1510002 cmp      r1, r2
00280138: 9afffffc bls      #0x280130
0028013c: eb0066d3 bl       #0x299c90
00280140: eb005d62 bl       #0x2976d0
00280144: eb006653 bl       #0x299a98
00280148: e59f13a0 ldr      r1, [pc, #0x3a0] ; [002804f0] = 0056c0a0 
0028014c: e1a0d001 mov      sp, r1
00280150: eb005ee4 bl       #0x297ce8
00280154: eafffffe b        #0x280154
00297ce8: e1a0c00d mov      ip, sp
00297cec: e92dd800 push     {fp, ip, lr, pc}
00297cf0: e24cb004 sub      fp, ip, #4
00297cf4: eb000002 bl       #0x297d04
00297cf8: eb000002 bl       #0x297d08
00297cfc: ebffa2b1 bl       #0x2807c8
00297d00: eb0290fd bl       #0x33c0fc
00297d04: e12fff1e bx       lr
00297d08: e12fff1e bx       lr
00299a98: e1a0c00d mov      ip, sp
00299a9c: e92dd830 push     {r4, r5, fp, ip, lr, pc}
00299aa0: e24cb004 sub      fp, ip, #4
00299aa4: e59f4020 ldr      r4, [pc, #0x20] ; [00299acc] = 0056ad58 
00299aa8: e59f3020 ldr      r3, [pc, #0x20] ; [00299ad0] = 0056ad08 
00299aac: e1540003 cmp      r4, r3
00299ab0: 389da830 ldmlo    sp, {r4, r5, fp, sp, pc}
00299ab4: e1a05003 mov      r5, r3
00299ab8: e1a0e00f mov      lr, pc
00299abc: e414f004 ldr      pc, [r4], #-4
00299ac0: e1540005 cmp      r4, r5
00299ac4: 2afffffb bhs      #0x299ab8
00299ac8: e89da830 ldm      sp, {r4, r5, fp, sp, pc}
00299acc: 0056ad58 subseq   sl, r6, r8, asr sp
00299ad0: 0056ad08 subseq   sl, r6, r8, lsl #26
00299c90: e1a0c00d mov      ip, sp
00299c94: e92dd880 push     {r7, fp, ip, lr, pc}
00299c98: e24cb004 sub      fp, ip, #4
00299c9c: eb000055 bl       #0x299df8
00299ca0: ebfff6bc bl       #0x297798
00299ca4: ee17ff7a mrc      p15, #0, apsr_nzcv, c7, c10, #3
00299ca8: 1afffffd bne      #0x299ca4
00299cac: e3a07000 mov      r7, #0
00299cb0: ee077f9a mcr      p15, #0, r7, c7, c10, #4
00299cb4: e3a00000 mov      r0, #0
00299cb8: ee070f16 mcr      p15, #0, r0, c7, c6, #0
00299cbc: ee080f17 mcr      p15, #0, r0, c8, c7, #0
00299cc0: e3a01000 mov      r1, #0
00299cc4: ee071f15 mcr      p15, #0, r1, c7, c5, #0
00299cc8: ee081f15 mcr      p15, #0, r1, c8, c5, #0
00299ccc: e1a00000 mov      r0, r0
00299cd0: e1a00000 mov      r0, r0
00299cd4: e1a00000 mov      r0, r0
00299cd8: e1a00000 mov      r0, r0
00299cdc: e1a00000 mov      r0, r0
00299ce0: e1a00000 mov      r0, r0
00299ce4: e89da880 ldm      sp, {r7, fp, sp, pc}
9d807180: e59f3010 ldr      r3, [pc, #0x10] ; [9d807198] = 9d807000 
9d807184: e593300c ldr      r3, [r3, #0xc]
9d807188: e3130802 tst      r3, #0x20000
9d80718c: 03a00000 moveq    r0, #0
9d807190: 13a00001 movne    r0, #1
9d807194: e12fff1e bx       lr
9d807198: 9d807000 stcls    p0, c7, [r0]
9d80719c: e12fff1e bx       lr
9d8071a0: e3500000 cmp      r0, #0
9d8071a4: 012fff1e bxeq     lr
9d8071a8: e2400001 sub      r0, r0, #1
9d8071ac: e3700001 cmn      r0, #1
9d8071b0: 1afffffc bne      #0x9d8071a8
9d8071b4: e12fff1e bx       lr
9d8071b8: e3a03492 mov      r3, #0x92000000
9d8071bc: e2833901 add      r3, r3, #0x4000
9d8071c0: e283301c add      r3, r3, #0x1c
9d8071c4: e3802c01 orr      r2, r0, #0x100
9d8071c8: e5832000 str      r2, [r3]
9d8071cc: e3a02492 mov      r2, #0x92000000
9d8071d0: e2822901 add      r2, r2, #0x4000
9d8071d4: e2822008 add      r2, r2, #8
9d8071d8: e5923000 ldr      r3, [r2]
9d8071dc: e383300a orr      r3, r3, #0xa
9d8071e0: e5823000 str      r3, [r2]
9d8071e4: e3a03000 mov      r3, #0
9d8071e8: e2833001 add      r3, r3, #1
9d8071ec: e3530009 cmp      r3, #9
9d8071f0: 9afffffc bls      #0x9d8071e8
9d8071f4: e3a03492 mov      r3, #0x92000000
9d8071f8: e2833901 add      r3, r3, #0x4000
9d8071fc: e283301c add      r3, r3, #0x1c
9d807200: e3c02c01 bic      r2, r0, #0x100
9d807204: e5832000 str      r2, [r3]
9d807208: e3a02492 mov      r2, #0x92000000
9d80720c: e2822901 add      r2, r2, #0x4000
9d807210: e2822008 add      r2, r2, #8
9d807214: e5923000 ldr      r3, [r2]
9d807218: e383300a orr      r3, r3, #0xa
9d80721c: e5823000 str      r3, [r2]
9d807220: e12fff1e bx       lr
