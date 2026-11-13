; SPMP-OS Stage55 — original MMI ARM instructions; read only
========= 0x2807c8 → 0x28083c =========
  2807c8: e1a0c00d     	mov	r12, sp
  2807cc: e92dd810     	push	{r4, r11, r12, lr, pc}
  2807d0: e24cb004     	sub	r11, r12, #4
  2807d4: e24dd010     	sub	sp, sp, #16
  2807d8: e59f0044     	ldr	r0, [pc, #0x44]         @ 0x280824
  2807dc: eb04f76f     	bl	0x3be5a0
  2807e0: e59f3040     	ldr	r3, [pc, #0x40]         @ 0x280828
  2807e4: e58d3000     	str	r3, [sp]
  2807e8: e3a03901     	mov	r3, #16384
  2807ec: e58d3004     	str	r3, [sp, #0x4]
  2807f0: e59f4034     	ldr	r4, [pc, #0x34]         @ 0x28082c
  2807f4: e58d4008     	str	r4, [sp, #0x8]
  2807f8: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x280830
  2807fc: e58d300c     	str	r3, [sp, #0xc]
  280800: e3a00019     	mov	r0, #25
  280804: e59f1028     	ldr	r1, [pc, #0x28]         @ 0x280834
  280808: e3a02000     	mov	r2, #0
  28080c: e59f3024     	ldr	r3, [pc, #0x24]         @ 0x280838
  280810: eb02e4a5     	bl	0x339aac
  280814: e5940000     	ldr	r0, [r4]
  280818: eb02e4d3     	bl	0x339b6c
  28081c: e24bd010     	sub	sp, r11, #16
  280820: e89da810     	ldm	sp, {r4, r11, sp, pc}
  280824: 002805f8     	<unknown>
  280828: 0056c0a0     	subseq	r12, r6, r0, lsr #1
  28082c: 00570140     	subseq	r0, r7, r0, asr #2
  280830: 005700a0     	subseq	r0, r7, r0, lsr #1
  280834: 003be5b0     	ldrhteq	lr, [r11], -r0
  280838: 004cfb44     	subeq	pc, r12, r4, asr #22
========= 0x3be5b0 → 0x3be6a0 =========
  3be5b0: e1a0c00d     	mov	r12, sp
  3be5b4: e92dd810     	push	{r4, r11, r12, lr, pc}
  3be5b8: e24cb004     	sub	r11, r12, #4
  3be5bc: e3a04001     	mov	r4, #1
  3be5c0: ebfded78     	bl	0x339ba8
  3be5c4: e3a0100a     	mov	r1, #10
  3be5c8: ebfded7a     	bl	0x339bb8
  3be5cc: eb02241b     	bl	0x447640
  3be5d0: e3500000     	cmp	r0, #0
  3be5d4: aa000003     	bge	0x3be5e8
  3be5d8: e59f00ac     	ldr	r0, [pc, #0xac]         @ 0x3be68c
  3be5dc: ebfb6960     	bl	0x298b64
  3be5e0: e1a00004     	mov	r0, r4
  3be5e4: eb0400dc     	bl	0x4be95c
  3be5e8: e1a00004     	mov	r0, r4
  3be5ec: e3a01000     	mov	r1, #0
  3be5f0: eb001e93     	bl	0x3c6044
  3be5f4: e3500000     	cmp	r0, #0
  3be5f8: b3a00001     	movlt	r0, #1
  3be5fc: bb0400d6     	bllt	0x4be95c
  3be600: e59f4088     	ldr	r4, [pc, #0x88]         @ 0x3be690
  3be604: e59f3088     	ldr	r3, [pc, #0x88]         @ 0x3be694
  3be608: e593305c     	ldr	r3, [r3, #0x5c]
  3be60c: e1a00004     	mov	r0, r4
  3be610: e1a0e00f     	mov	lr, pc
  3be614: e1a0f003     	mov	pc, r3
  3be618: e3500000     	cmp	r0, #0
  3be61c: 0a00000a     	beq	0x3be64c
  3be620: e5943004     	ldr	r3, [r4, #0x4]
  3be624: e3130010     	tst	r3, #16
  3be628: 0a000007     	beq	0x3be64c
  3be62c: e5943018     	ldr	r3, [r4, #0x18]
  3be630: e3130302     	tst	r3, #134217728
  3be634: 0a000004     	beq	0x3be64c
  3be638: e3a00000     	mov	r0, #0
  3be63c: ebfb8b91     	bl	0x2a1488
  3be640: ebfb89e0     	bl	0x2a0dc8
  3be644: e59f004c     	ldr	r0, [pc, #0x4c]         @ 0x3be698
  3be648: ebfb6945     	bl	0x298b64
  3be64c: eb000013     	bl	0x3be6a0
  3be650: e3500000     	cmp	r0, #0
  3be654: b3a00001     	movlt	r0, #1
  3be658: bb0400bf     	bllt	0x4be95c
  3be65c: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x3be69c
  3be660: e593c000     	ldr	r12, [r3]
  3be664: e35c0000     	cmp	r12, #0
  3be668: 0a000005     	beq	0x3be684
  3be66c: e3a00000     	mov	r0, #0
  3be670: e1a01000     	mov	r1, r0
  3be674: e1a02000     	mov	r2, r0
  3be678: e3a03005     	mov	r3, #5
  3be67c: e1a0e00f     	mov	lr, pc
  3be680: e1a0f00c     	mov	pc, r12
  3be684: eb00000d     	bl	0x3be6c0
  3be688: e89da810     	ldm	sp, {r4, r11, sp, pc}
  3be68c: 004f75a4     	subeq	r7, pc, r4, lsr #11
  3be690: 0066afa0     	rsbeq	r10, r6, r0, lsr #31
  3be694: 00000050     	andeq	r0, r0, r0, asr r0
  3be698: 004f75c0     	subeq	r7, pc, r0, asr #11
  3be69c: 00560c18     	subseq	r0, r6, r8, lsl r12
========= 0x4473bc → 0x447458 =========
  4473bc: e1a0c00d     	mov	r12, sp
  4473c0: e92dd830     	push	{r4, r5, r11, r12, lr, pc}
  4473c4: e24cb004     	sub	r11, r12, #4
  4473c8: e3a00000     	mov	r0, #0
  4473cc: ebffc3f0     	bl	0x438394
  4473d0: e3a0000a     	mov	r0, #10
  4473d4: ebf95d15     	bl	0x29e830
  4473d8: e3a00000     	mov	r0, #0
  4473dc: ebffffe1     	bl	0x447368
  4473e0: e3a00000     	mov	r0, #0
  4473e4: ebffffe7     	bl	0x447388
  4473e8: ebffffb5     	bl	0x4472c4
  4473ec: e3a05001     	mov	r5, #1
  4473f0: ebfa19b5     	bl	0x2cdacc
  4473f4: e3500000     	cmp	r0, #0
  4473f8: 1a000001     	bne	0x447404
  4473fc: ebfa19c4     	bl	0x2cdb14
  447400: ea00000e     	b	0x447440
  447404: e59f4048     	ldr	r4, [pc, #0x48]         @ 0x447454
  447408: e5943000     	ldr	r3, [r4]
  44740c: e3530000     	cmp	r3, #0
  447410: 1a000006     	bne	0x447430
  447414: ebfa4510     	bl	0x2d885c
  447418: e3500000     	cmp	r0, #0
  44741c: 1a000003     	bne	0x447430
  447420: e280000d     	add	r0, r0, #13
  447424: ebfa44d6     	bl	0x2d8784
  447428: e5845000     	str	r5, [r4]
  44742c: ea000001     	b	0x447438
  447430: e59f301c     	ldr	r3, [pc, #0x1c]         @ 0x447454
  447434: e5835000     	str	r5, [r3]
  447438: ebfa13e9     	bl	0x2cc3e4
  44743c: ebfa1412     	bl	0x2cc48c
  447440: ebfa1a10     	bl	0x2cdc88
  447444: e3a01000     	mov	r1, #0
  447448: e3a00064     	mov	r0, #100
  44744c: ebfbc9e5     	bl	0x339be8
  447450: eaffffe6     	b	0x4473f0
  447454: 005623f4     	ldrsheq	r2, [r6], #-52
========= 0x447458 → 0x4474d8 =========
  447458: e1a0c00d     	mov	r12, sp
  44745c: e92dd810     	push	{r4, r11, r12, lr, pc}
  447460: e24cb004     	sub	r11, r12, #4
  447464: e24dd010     	sub	sp, sp, #16
  447468: e59f4050     	ldr	r4, [pc, #0x50]         @ 0x4474c0
  44746c: e5942000     	ldr	r2, [r4]
  447470: e3520000     	cmp	r2, #0
  447474: 1a00000c     	bne	0x4474ac
  447478: e59f3044     	ldr	r3, [pc, #0x44]         @ 0x4474c4
  44747c: e58d3000     	str	r3, [sp]
  447480: e3a03da6     	mov	r3, #10624
  447484: e98d0018     	stmib	sp, {r3, r4}
  447488: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x4474c8
  44748c: e58d300c     	str	r3, [sp, #0xc]
  447490: e3a0001e     	mov	r0, #30
  447494: e59f1030     	ldr	r1, [pc, #0x30]         @ 0x4474cc
  447498: e59f3030     	ldr	r3, [pc, #0x30]         @ 0x4474d0
  44749c: ebfbc982     	bl	0x339aac
  4474a0: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x4474d4
  4474a4: e5941000     	ldr	r1, [r4]
  4474a8: ebf945ad     	bl	0x298b64
  4474ac: e59f300c     	ldr	r3, [pc, #0xc]          @ 0x4474c0
  4474b0: e5930000     	ldr	r0, [r3]
  4474b4: ebfbc9ac     	bl	0x339b6c
  4474b8: e24bd010     	sub	sp, r11, #16
  4474bc: e89da810     	ldm	sp, {r4, r11, sp, pc}
  4474c0: 0069391c     	rsbeq	r3, r9, r12, lsl r9
  4474c4: 00690efc     	<unknown>
  4474c8: 0069387c     	rsbeq	r3, r9, r12, ror r8
  4474cc: 004473bc     	strheq	r7, [r4], #-60
  4474d0: 00504214     	subseq	r4, r0, r4, lsl r2
  4474d4: 00504230     	subseq	r4, r0, r0, lsr r2
========= 0x4474d8 → 0x447540 =========
  4474d8: e1a0c00d     	mov	r12, sp
  4474dc: e92dd830     	push	{r4, r5, r11, r12, lr, pc}
  4474e0: e24cb004     	sub	r11, r12, #4
  4474e4: e3a04000     	mov	r4, #0
  4474e8: e3a0500a     	mov	r5, #10
  4474ec: e59f0040     	ldr	r0, [pc, #0x40]         @ 0x447534
  4474f0: ebf9459b     	bl	0x298b64
  4474f4: e3a00002     	mov	r0, #2
  4474f8: ebffc3a5     	bl	0x438394
  4474fc: e3500000     	cmp	r0, #0
  447500: 0a000007     	beq	0x447524
  447504: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x447538
  447508: ebf94595     	bl	0x298b64
  44750c: e2844001     	add	r4, r4, #1
  447510: e3a01000     	mov	r1, #0
  447514: e3a0000a     	mov	r0, #10
  447518: ebfbc9b2     	bl	0x339be8
  44751c: e1540005     	cmp	r4, r5
  447520: bafffff3     	blt	0x4474f4
  447524: e59f0010     	ldr	r0, [pc, #0x10]         @ 0x44753c
  447528: e3a01001     	mov	r1, #1
  44752c: ebfbca82     	bl	0x339f3c
  447530: e89da830     	ldm	sp, {r4, r5, r11, sp, pc}
  447534: 0050425c     	subseq	r4, r0, r12, asr r2
  447538: 00504284     	subseq	r4, r0, r4, lsl #5
  44753c: 0068e4d0     	<unknown>
========= 0x447540 → 0x4475c0 =========
  447540: e1a0c00d     	mov	r12, sp
  447544: e92dd800     	push	{r11, r12, lr, pc}
  447548: e24cb004     	sub	r11, r12, #4
  44754c: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x4475b0
  447550: ebfbca6e     	bl	0x339f10
  447554: e59f0058     	ldr	r0, [pc, #0x58]         @ 0x4475b4
  447558: ebfbca6c     	bl	0x339f10
  44755c: ebf90463     	bl	0x2886f0
  447560: e59f004c     	ldr	r0, [pc, #0x4c]         @ 0x4475b4
  447564: e3a01001     	mov	r1, #1
  447568: e3a02003     	mov	r2, #3
  44756c: ebfbca7c     	bl	0x339f64
  447570: e59f3040     	ldr	r3, [pc, #0x40]         @ 0x4475b8
  447574: e5933000     	ldr	r3, [r3]
  447578: e3530001     	cmp	r3, #1
  44757c: 1bfa2969     	blne	0x2d1b28
  447580: e59f0034     	ldr	r0, [pc, #0x34]         @ 0x4475bc
  447584: e3a01001     	mov	r1, #1
  447588: ebfbca6b     	bl	0x339f3c
  44758c: ebffffd1     	bl	0x4474d8
  447590: e3a00d16     	mov	r0, #1408
  447594: e2800001     	add	r0, r0, #1
  447598: e3a01000     	mov	r1, #0
  44759c: e1a02001     	mov	r2, r1
  4475a0: ebfa4578     	bl	0x2d8b88
  4475a4: ebffffab     	bl	0x447458
  4475a8: ebfbc955     	bl	0x339b04
  4475ac: e89da800     	ldm	sp, {r11, sp, pc}
  4475b0: 0068e4d0     	<unknown>
  4475b4: 0068e4c8     	rsbeq	lr, r8, r8, asr #9
  4475b8: 0055613c     	subseq	r6, r5, r12, lsr r1
  4475bc: 005ef634     	subseq	pc, lr, r4, lsr r6
========= 0x4475c0 → 0x447640 =========
  4475c0: e1a0c00d     	mov	r12, sp
  4475c4: e92dd830     	push	{r4, r5, r11, r12, lr, pc}
  4475c8: e24cb004     	sub	r11, r12, #4
  4475cc: e24dd010     	sub	sp, sp, #16
  4475d0: e59f5050     	ldr	r5, [pc, #0x50]         @ 0x447628
  4475d4: e5954000     	ldr	r4, [r5]
  4475d8: e3540000     	cmp	r4, #0
  4475dc: 1a00000c     	bne	0x447614
  4475e0: e59f0044     	ldr	r0, [pc, #0x44]         @ 0x44762c
  4475e4: ebf9455e     	bl	0x298b64
  4475e8: e59f3040     	ldr	r3, [pc, #0x40]         @ 0x447630
  4475ec: e58d3000     	str	r3, [sp]
  4475f0: e3a03da6     	mov	r3, #10624
  4475f4: e98d0028     	stmib	sp, {r3, r5}
  4475f8: e59f3034     	ldr	r3, [pc, #0x34]         @ 0x447634
  4475fc: e58d300c     	str	r3, [sp, #0xc]
  447600: e3a0000c     	mov	r0, #12
  447604: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0x447638
  447608: e1a02004     	mov	r2, r4
  44760c: e59f3028     	ldr	r3, [pc, #0x28]         @ 0x44763c
  447610: ebfbc925     	bl	0x339aac
  447614: e59f300c     	ldr	r3, [pc, #0xc]          @ 0x447628
  447618: e5930000     	ldr	r0, [r3]
  44761c: ebfbc952     	bl	0x339b6c
  447620: e24bd014     	sub	sp, r11, #20
  447624: e89da830     	ldm	sp, {r4, r5, r11, sp, pc}
  447628: 00690ef8     	<unknown>
  44762c: 005042ac     	subseq	r4, r0, r12, lsr #5
  447630: 0068e4d8     	<unknown>
  447634: 00690e58     	rsbeq	r0, r9, r8, asr lr
  447638: 00447540     	subeq	r7, r4, r0, asr #10
  44763c: 005042dc     	ldrsbeq	r4, [r0], #-44
========= 0x447640 → 0x4476cc =========
  447640: e1a0c00d     	mov	r12, sp
  447644: e92dd800     	push	{r11, r12, lr, pc}
  447648: e24cb004     	sub	r11, r12, #4
  44764c: e59f006c     	ldr	r0, [pc, #0x6c]         @ 0x4476c0
  447650: e59f106c     	ldr	r1, [pc, #0x6c]         @ 0x4476c4
  447654: ebf94542     	bl	0x298b64
  447658: e3a0000a     	mov	r0, #10
  44765c: e3a01000     	mov	r1, #0
  447660: ebfa4ca0     	bl	0x2da8e8
  447664: ebfa4859     	bl	0x2d97d0
  447668: e3a02209     	mov	r2, #-1879048192
  44766c: e2822a05     	add	r2, r2, #20480
  447670: e2822040     	add	r2, r2, #64
  447674: e5923000     	ldr	r3, [r2]
  447678: e3833007     	orr	r3, r3, #7
  44767c: e5823000     	str	r3, [r2]
  447680: ebffec8d     	bl	0x4428bc
  447684: eb020cee     	bl	0x4caa44
  447688: e3500000     	cmp	r0, #0
  44768c: 0b020ec2     	bleq	0x4cb19c
  447690: e59f0030     	ldr	r0, [pc, #0x30]         @ 0x4476c8
  447694: ebfbca61     	bl	0x33a020
  447698: ebfa37bf     	bl	0x2d559c
  44769c: ebf9c8df     	bl	0x2b9a20
  4476a0: eb019bf8     	bl	0x4ae688
  4476a4: ebfc1803     	bl	0x34d6b8
  4476a8: ebfbc4cc     	bl	0x3389e0
  4476ac: ebf92b10     	bl	0x2922f4
  4476b0: ebffffc2     	bl	0x4475c0
  4476b4: ebfe6cb6     	bl	0x3e2994
  4476b8: e3a00000     	mov	r0, #0
  4476bc: e89da800     	ldm	sp, {r11, sp, pc}
  4476c0: 004e0848     	subeq	r0, lr, r8, asr #16
  4476c4: 00504300     	subseq	r4, r0, r0, lsl #6
  4476c8: 0068e4b4     	strhteq	lr, [r8], #-68
========= 0x339aac → 0x339b04 =========
  339aac: e1a0c00d     	mov	r12, sp
  339ab0: e92dd830     	push	{r4, r5, r11, r12, lr, pc}
  339ab4: e24cb004     	sub	r11, r12, #4
  339ab8: e24dd00c     	sub	sp, sp, #12
  339abc: e1a0c000     	mov	r12, r0
  339ac0: e1a0e001     	mov	lr, r1
  339ac4: e1a05002     	mov	r5, r2
  339ac8: e59b4010     	ldr	r4, [r11, #0x10]
  339acc: e58d3000     	str	r3, [sp]
  339ad0: e59b3004     	ldr	r3, [r11, #0x4]
  339ad4: e58d3004     	str	r3, [sp, #0x4]
  339ad8: e59b3008     	ldr	r3, [r11, #0x8]
  339adc: e58d3008     	str	r3, [sp, #0x8]
  339ae0: e1a00004     	mov	r0, r4
  339ae4: e1a0100c     	mov	r1, r12
  339ae8: e1a0200e     	mov	r2, lr
  339aec: e1a03005     	mov	r3, r5
  339af0: eb0001f9     	bl	0x33a2dc
  339af4: e59b300c     	ldr	r3, [r11, #0xc]
  339af8: e5834000     	str	r4, [r3]
  339afc: e24bd014     	sub	sp, r11, #20
  339b00: e89da830     	ldm	sp, {r4, r5, r11, sp, pc}
