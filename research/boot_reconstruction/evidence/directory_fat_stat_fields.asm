; Selected FAT stat field stores, exact 8000_MMI.RAP SHA 5529b173b3fb612b2e7f516af807f1342d24ea6f7101be034e1e0d08fed25c4e
00283d5c: e3a02000 mov      r2, #0
00283d60: e51b3038 ldr      r3, [fp, #-0x38]
00283d64: e3130001 tst      r3, #1
00283d68: 13822001 orrne    r2, r2, #1
00283d6c: e51b3038 ldr      r3, [fp, #-0x38]
00283d70: e3130002 tst      r3, #2
00283d74: 13822008 orrne    r2, r2, #8
00283d78: e51b3038 ldr      r3, [fp, #-0x38]
00283d7c: e3130004 tst      r3, #4
00283d80: 13822849 orrne    r2, r2, #0x490000
00283d84: e51b3038 ldr      r3, [fp, #-0x38]
00283d88: e3130008 tst      r3, #8
00283d8c: 13822892 orrne    r2, r2, #0x920000
00283d90: e3822749 orr      r2, r2, #0x1240000
00283d94: e5842000 str      r2, [r4]
00283d98: e3a02000 mov      r2, #0
00283d9c: e1c420b8 strh     r2, [r4, #8]
00283da0: e51b3034 ldr      r3, [fp, #-0x34]
00283da4: e5843004 str      r3, [r4, #4]
00283da8: e3a03001 mov      r3, #1
00283dac: e1c430ba strh     r3, [r4, #0xa]
00283db0: e1c420bc strh     r2, [r4, #0xc]
00283db4: e51b3024 ldr      r3, [fp, #-0x24]
00283db8: e5843014 str      r3, [r4, #0x14]
00283dbc: e51b3020 ldr      r3, [fp, #-0x20]
00283dc0: e5843018 str      r3, [r4, #0x18]
00283dc4: e51b301c ldr      r3, [fp, #-0x1c]
00283dc8: e584301c str      r3, [r4, #0x1c]
00283dcc: e51b3028 ldr      r3, [fp, #-0x28]
00283dd0: e5843010 str      r3, [r4, #0x10]
00283dd4: e3a00000 mov      r0, #0
