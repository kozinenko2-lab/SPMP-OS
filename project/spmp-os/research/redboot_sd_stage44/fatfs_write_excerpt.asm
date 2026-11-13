
/mnt/data/SPMP-OS_Stage44_SDMMCRestore/rap_for_disasm.elf:	file format elf32-littlearm

Disassembly of section .text:

00280000 <_start>:
  283e90: e5940000     	ldr	r0, [r4]
  283e94: e51b101c     	ldr	r1, [r11, #-0x1c]
  283e98: e2462001     	sub	r2, r6, #1
  283e9c: eb03091f     	bl	0x346320 <_start+0xc6320> @ imm = #0xc247c
  283ea0: e3a00000     	mov	r0, #0
  283ea4: e99da870     	ldmib	sp, {r4, r5, r6, r11, sp, pc}
  283ea8: 004d02a4     	subeq	r0, sp, r4, lsr #5
  283eac: 004d02c0     	subeq	r0, sp, r0, asr #5
  283eb0: 004d02e0     	subeq	r0, sp, r0, ror #5
  283eb4: 005704c8     	subseq	r0, r7, r8, asr #9
  283eb8: 004d00c0     	subeq	r0, sp, r0, asr #1
  283ebc: 004d3cdc     	ldrdeq	r3, r4, [sp], #-204
  283ec0: e1a0c00d     	mov	r12, sp
  283ec4: e92dd870     	push	{r4, r5, r6, r11, r12, lr, pc}
  283ec8: e24cb004     	sub	r11, r12, #4
  283ecc: e24dd004     	sub	sp, sp, #4
  283ed0: e1a04002     	mov	r4, r2
  283ed4: e1a05003     	mov	r5, r3
  283ed8: e3a03000     	mov	r3, #0
  283edc: e50b301c     	str	r3, [r11, #-0x1c]
  283ee0: e59f0044     	ldr	r0, [pc, #0x44]         @ 0x283f2c <_start+0x3f2c>
  283ee4: eb00531e     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14c78
  283ee8: e3a06016     	mov	r6, #22
  283eec: e24b0018     	sub	r0, r11, #24
  283ef0: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x283f30 <_start+0x3f30>
  283ef4: e5203004     	str	r3, [r0, #-0x4]!
  283ef8: e1a01004     	mov	r1, r4
  283efc: ebfffd76     	bl	0x2834dc <_start+0x34dc> @ imm = #-0xa28
  283f00: e3550004     	cmp	r5, #4
  283f04: 1a000002     	bne	0x283f14 <_start+0x3f14> @ imm = #0x8
  283f08: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x283f34 <_start+0x3f34>
  283f0c: eb005314     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14c50
  283f10: ea000003     	b	0x283f24 <_start+0x3f24> @ imm = #0xc
  283f14: e59f001c     	ldr	r0, [pc, #0x1c]         @ 0x283f38 <_start+0x3f38>
  283f18: e1a01005     	mov	r1, r5
  283f1c: eb005310     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14c40
  283f20: e3a06016     	mov	r6, #22
  283f24: e1a00006     	mov	r0, r6
  283f28: e99da870     	ldmib	sp, {r4, r5, r6, r11, sp, pc}
  283f2c: 004d0324     	subeq	r0, sp, r4, lsr #6
  283f30: 005704c8     	subseq	r0, r7, r8, asr #9
  283f34: 004d0348     	subeq	r0, sp, r8, asr #6
  283f38: 004d0384     	subeq	r0, sp, r4, lsl #7
  283f3c: e1a0c00d     	mov	r12, sp
  283f40: e92dd830     	push	{r4, r5, r11, r12, lr, pc}
  283f44: e24cb004     	sub	r11, r12, #4
  283f48: e1a04001     	mov	r4, r1
  283f4c: e5900014     	ldr	r0, [r0, #0x14]
  283f50: e3a05016     	mov	r5, #22
  283f54: e3500040     	cmp	r0, #64
  283f58: 1a000003     	bne	0x283f6c <_start+0x3f6c> @ imm = #0xc
  283f5c: e59f0054     	ldr	r0, [pc, #0x54]         @ 0x283fb8 <_start+0x3fb8>
  283f60: eb0052ff     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14bfc
  283f64: e1a00005     	mov	r0, r5
  283f68: e89da830     	ldm	sp, {r4, r5, r11, sp, pc}
  283f6c: e5913000     	ldr	r3, [r1]
  283f70: e8930006     	ldm	r3, {r1, r2}
  283f74: eb0094df     	bl	0x2a92f8 <_start+0x292f8> @ imm = #0x2537c
  283f78: e3500000     	cmp	r0, #0
  283f7c: a594300c     	ldrge	r3, [r4, #0xc]
  283f80: a0603003     	rsbge	r3, r0, r3
  283f84: a584300c     	strge	r3, [r4, #0xc]
  283f88: a3a05000     	movge	r5, #0
  283f8c: aa000007     	bge	0x283fb0 <_start+0x3fb0> @ imm = #0x1c
  283f90: eb009c76     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x271d8
  283f94: e1a03000     	mov	r3, r0
  283f98: e59f001c     	ldr	r0, [pc, #0x1c]         @ 0x283fbc <_start+0x3fbc>
  283f9c: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x283fc0 <_start+0x3fc0>
  283fa0: e3a02e4a     	mov	r2, #1184
  283fa4: eb0052ee     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14bb8
  283fa8: eb009c70     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x271c0
  283fac: e1a05000     	mov	r5, r0
  283fb0: e1a00005     	mov	r0, r5
  283fb4: e89da830     	ldm	sp, {r4, r5, r11, sp, pc}
  283fb8: 004d03b8     	strheq	r0, [sp], #-56
  283fbc: 004d00c0     	subeq	r0, sp, r0, asr #1
  283fc0: 004d3d08     	subeq	r3, sp, r8, lsl #26
  283fc4: e1a0c00d     	mov	r12, sp
  283fc8: e92dd830     	push	{r4, r5, r11, r12, lr, pc}
  283fcc: e24cb004     	sub	r11, r12, #4
  283fd0: e1a04001     	mov	r4, r1
  283fd4: e5901014     	ldr	r1, [r0, #0x14]
  283fd8: e3a05016     	mov	r5, #22
  283fdc: e351003f     	cmp	r1, #63
  283fe0: da000003     	ble	0x283ff4 <_start+0x3ff4> @ imm = #0xc
  283fe4: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x284048 <_start+0x4048>
  283fe8: eb0052dd     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14b74
  283fec: e1a00005     	mov	r0, r5
  283ff0: e89da830     	ldm	sp, {r4, r5, r11, sp, pc}
  283ff4: e5943000     	ldr	r3, [r4]
  283ff8: e1a00001     	mov	r0, r1
  283ffc: e8930006     	ldm	r3, {r1, r2}
  284000: eb009563     	bl	0x2a9594 <_start+0x29594> @ imm = #0x2558c
  284004: e3500000     	cmp	r0, #0
  284008: a594300c     	ldrge	r3, [r4, #0xc]
  28400c: a0603003     	rsbge	r3, r0, r3
  284010: a584300c     	strge	r3, [r4, #0xc]
  284014: a3a05000     	movge	r5, #0
  284018: aa000008     	bge	0x284040 <_start+0x4040> @ imm = #0x20
  28401c: eb009c53     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x2714c
  284020: e1a03000     	mov	r3, r0
  284024: e59f0020     	ldr	r0, [pc, #0x20]         @ 0x28404c <_start+0x404c>
  284028: e59f1020     	ldr	r1, [pc, #0x20]         @ 0x284050 <_start+0x4050>
  28402c: e3a02d13     	mov	r2, #1216
  284030: e2822002     	add	r2, r2, #2
  284034: eb0052ca     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14b28
  284038: eb009c4c     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x27130
  28403c: e1a05000     	mov	r5, r0
  284040: e1a00005     	mov	r0, r5
  284044: e89da830     	ldm	sp, {r4, r5, r11, sp, pc}
  284048: 004d03e8     	subeq	r0, sp, r8, ror #7
  28404c: 004d00c0     	subeq	r0, sp, r0, asr #1
  284050: 004d3d34     	subeq	r3, sp, r4, lsr sp
  284054: e1a0c00d     	mov	r12, sp
  284058: e92dd870     	push	{r4, r5, r6, r11, r12, lr, pc}
  28405c: e24cb004     	sub	r11, r12, #4
  284060: e1a05000     	mov	r5, r0
  284064: e1a06001     	mov	r6, r1
  284068: e5901014     	ldr	r1, [r0, #0x14]
  28406c: e3a04016     	mov	r4, #22
  284070: e351003f     	cmp	r1, #63
  284074: da000003     	ble	0x284088 <_start+0x4088> @ imm = #0xc
  284078: e59f0054     	ldr	r0, [pc, #0x54]         @ 0x2840d4 <_start+0x40d4>
  28407c: eb0052b8     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14ae0
  284080: e1a00004     	mov	r0, r4
  284084: e89da870     	ldm	sp, {r4, r5, r6, r11, sp, pc}
  284088: e1a00001     	mov	r0, r1
  28408c: e5961000     	ldr	r1, [r6]
  284090: eb0095e7     	bl	0x2a9834 <_start+0x29834> @ imm = #0x2579c
  284094: e3700001     	cmn	r0, #1
  284098: 15850010     	strne	r0, [r5, #0x10]
  28409c: 15860000     	strne	r0, [r6]
  2840a0: 13a04000     	movne	r4, #0
  2840a4: 1a000008     	bne	0x2840cc <_start+0x40cc> @ imm = #0x20
  2840a8: eb009c30     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x270c0
  2840ac: e1a03000     	mov	r3, r0
  2840b0: e59f0020     	ldr	r0, [pc, #0x20]         @ 0x2840d8 <_start+0x40d8>
  2840b4: e59f1020     	ldr	r1, [pc, #0x20]         @ 0x2840dc <_start+0x40dc>
  2840b8: e3a02e4e     	mov	r2, #1248
  2840bc: e282200a     	add	r2, r2, #10
  2840c0: eb0052a7     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14a9c
  2840c4: eb009c29     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x270a4
  2840c8: e1a04000     	mov	r4, r0
  2840cc: e1a00004     	mov	r0, r4
  2840d0: e89da870     	ldm	sp, {r4, r5, r6, r11, sp, pc}
  2840d4: 004d0420     	subeq	r0, sp, r0, lsr #8
  2840d8: 004d00c0     	subeq	r0, sp, r0, asr #1
  2840dc: 004d3d60     	subeq	r3, sp, r0, ror #26
  2840e0: e1a0c00d     	mov	r12, sp
  2840e4: e92dd800     	push	{r11, r12, lr, pc}
  2840e8: e24cb004     	sub	r11, r12, #4
  2840ec: e59f0008     	ldr	r0, [pc, #0x8]          @ 0x2840fc <_start+0x40fc>
  2840f0: eb00529b     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14a6c
  2840f4: e3a00016     	mov	r0, #22
  2840f8: e89da800     	ldm	sp, {r11, sp, pc}
  2840fc: 004d0464     	subeq	r0, sp, r4, ror #8
  284100: e3a00000     	mov	r0, #0
  284104: e12fff1e     	bx	lr
  284108: e1a0c00d     	mov	r12, sp
  28410c: e92dd800     	push	{r11, r12, lr, pc}
  284110: e24cb004     	sub	r11, r12, #4
  284114: e1a01000     	mov	r1, r0
  284118: e5900014     	ldr	r0, [r0, #0x14]
  28411c: e350003f     	cmp	r0, #63
  284120: da000003     	ble	0x284134 <_start+0x4134> @ imm = #0xc
  284124: e59f0050     	ldr	r0, [pc, #0x50]         @ 0x28417c <_start+0x417c>
  284128: eb00528d     	bl	0x298b64 <_start+0x18b64> @ imm = #0x14a34
  28412c: e3a00016     	mov	r0, #22
  284130: e89da800     	ldm	sp, {r11, sp, pc}
  284134: eb0093ec     	bl	0x2a90ec <_start+0x290ec> @ imm = #0x24fb0
  284138: e3500000     	cmp	r0, #0
  28413c: a3a00000     	movge	r0, #0
  284140: a89da800     	ldmge	sp, {r11, sp, pc}
  284144: e3700003     	cmn	r0, #3
  284148: 1a000002     	bne	0x284158 <_start+0x4158> @ imm = #0x8
  28414c: eb032304     	bl	0x34cd64 <_start+0xccd64> @ imm = #0xc8c10
  284150: e3500000     	cmp	r0, #0
  284154: 089da800     	ldmeq	sp, {r11, sp, pc}
  284158: eb009c04     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x27010
  28415c: e1a03000     	mov	r3, r0
  284160: e59f0018     	ldr	r0, [pc, #0x18]         @ 0x284180 <_start+0x4180>
  284164: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x284184 <_start+0x4184>
  284168: e3a02e52     	mov	r2, #1312
  28416c: e2822008     	add	r2, r2, #8
  284170: eb00527b     	bl	0x298b64 <_start+0x18b64> @ imm = #0x149ec
  284174: eb009bfd     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x26ff4
  284178: e89da800     	ldm	sp, {r11, sp, pc}
  28417c: 004d0494     	<unknown>
  284180: 004d00c0     	subeq	r0, sp, r0, asr #1
  284184: 004d3d7c     	subeq	r3, sp, r12, ror sp
  284188: e1a0c00d     	mov	r12, sp
  28418c: e92dd810     	push	{r4, r11, r12, lr, pc}
  284190: e24cb004     	sub	r11, r12, #4
  284194: e24dd024     	sub	sp, sp, #36
  284198: e1a04001     	mov	r4, r1
  28419c: e5900014     	ldr	r0, [r0, #0x14]
  2841a0: e350003f     	cmp	r0, #63
  2841a4: da000003     	ble	0x2841b8 <_start+0x41b8> @ imm = #0xc
  2841a8: e59f00cc     	ldr	r0, [pc, #0xcc]         @ 0x28427c <_start+0x427c>
  2841ac: eb00526c     	bl	0x298b64 <_start+0x18b64> @ imm = #0x149b0
  2841b0: e3a00016     	mov	r0, #22
  2841b4: ea00002e     	b	0x284274 <_start+0x4274> @ imm = #0xb8
  2841b8: e24b1034     	sub	r1, r11, #52
  2841bc: eb009340     	bl	0x2a8ec4 <_start+0x28ec4> @ imm = #0x24d00
  2841c0: e3500000     	cmp	r0, #0
  2841c4: 0a000008     	beq	0x2841ec <_start+0x41ec> @ imm = #0x20
  2841c8: eb009be8     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x26fa0
  2841cc: e1a03000     	mov	r3, r0
  2841d0: e59f00a8     	ldr	r0, [pc, #0xa8]         @ 0x284280 <_start+0x4280>
  2841d4: e59f10a8     	ldr	r1, [pc, #0xa8]         @ 0x284284 <_start+0x4284>
  2841d8: e3a02d15     	mov	r2, #1344
  2841dc: e282200e     	add	r2, r2, #14
  2841e0: eb00525f     	bl	0x298b64 <_start+0x18b64> @ imm = #0x1497c
  2841e4: eb009be1     	bl	0x2ab170 <_start+0x2b170> @ imm = #0x26f84
  2841e8: ea000021     	b	0x284274 <_start+0x4274> @ imm = #0x84
  2841ec: e3a02000     	mov	r2, #0
  2841f0: e51b3034     	ldr	r3, [r11, #-0x34]
  2841f4: e3130001     	tst	r3, #1
  2841f8: 13822001     	orrne	r2, r2, #1
  2841fc: e51b3034     	ldr	r3, [r11, #-0x34]
  284200: e3130002     	tst	r3, #2
  284204: 13822008     	orrne	r2, r2, #8
  284208: e51b3034     	ldr	r3, [r11, #-0x34]
  28420c: e3130004     	tst	r3, #4
  284210: 13822849     	orrne	r2, r2, #4784128
  284214: e51b3034     	ldr	r3, [r11, #-0x34]
  284218: e3130008     	tst	r3, #8
  28421c: 13822892     	orrne	r2, r2, #9568256
  284220: e3822749     	orr	r2, r2, #19136512
  284224: e5842000     	str	r2, [r4]
  284228: e51b3030     	ldr	r3, [r11, #-0x30]
  28422c: e5843004     	str	r3, [r4, #0x4]
  284230: e15b32bc     	ldrh	r3, [r11, #-44]
  284234: e1c430b8     	strh	r3, [r4, #8]
  284238: e15b32ba     	ldrh	r3, [r11, #-42]
  28423c: e1c430ba     	strh	r3, [r4, #10]
  284240: e15b32b8     	ldrh	r3, [r11, #-40]
  284244: e1c430bc     	strh	r3, [r4, #12]
  284248: e15b32b6     	ldrh	r3, [r11, #-38]
  28424c: e1c430be     	strh	r3, [r4, #14]
  284250: e51b3024     	ldr	r3, [r11, #-0x24]
  284254: e5843010     	str	r3, [r4, #0x10]
  284258: e51b3020     	ldr	r3, [r11, #-0x20]
  28425c: e5843014     	str	r3, [r4, #0x14]
  284260: e51b301c     	ldr	r3, [r11, #-0x1c]
  284264: e5843018     	str	r3, [r4, #0x18]
  284268: e51b3018     	ldr	r3, [r11, #-0x18]
  28426c: e584301c     	str	r3, [r4, #0x1c]
  284270: e3a00000     	mov	r0, #0
  284274: e24bd010     	sub	sp, r11, #16
  284278: e89da810     	ldm	sp, {r4, r11, sp, pc}
  28427c: 004d03b8     	strheq	r0, [sp], #-56
  284280: 004d00c0     	subeq	r0, sp, r0, asr #1
  284284: 004d3da0     	subeq	r3, sp, r0, lsr #27
  284288: e1a0c00d     	mov	r12, sp
  28428c: e92dd800     	push	{r11, r12, lr, pc}
  284290: e24cb004     	sub	r11, r12, #4
  284294: e59f0008     	ldr	r0, [pc, #0x8]          @ 0x2842a4 <_start+0x42a4>
  284298: eb005231     	bl	0x298b64 <_start+0x18b64> @ imm = #0x148c4
  28429c: e3a00016     	mov	r0, #22
