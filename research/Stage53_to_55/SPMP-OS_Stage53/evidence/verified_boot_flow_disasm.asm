[MMI: reset/CPU/stack]

/mnt/data/spmp_stage53/work/SPMP-OS_Stage52_Ghidra_Imports/elf/MMI/ghidra_analysis_ONLY.elf:	file format elf32-littlearm

Disassembly of section .spmp_seg_0:

00280040 <MMI_reset_00280040>:
  280040:      	mrc	p15, #0x0, r0, c1, c0, #0x0
  280044:      	mvn	r1, #8192
  280048:      	and	r0, r0, r1
  28004c:      	mcr	p15, #0x0, r0, c1, c0, #0x0
  280050:      	mrs	r7, apsr
  280054:      	and	r7, r7, #31
  280058:      	cmp	r7, #19
  28005c:      	beq	0x280080 <MMI_reset_00280040+0x40> @ imm = #0x1c
  280060:      	mov	r0, #0
  280064:      	ldr	r1, [pc, #0x480]        @ 0x2804ec <MMI_reset_00280040+0x4ac>
  280068:      	ldr	r2, [r1, #0x4]
  28006c:      	str	r2, [r0, #0x4]
  280070:      	ldr	r2, [r1, #0x24]
  280074:      	str	r2, [r0, #0x24]
  280078:      	ldr	r2, [r1, #0x8]
  28007c:      	str	r2, [r0, #0x8]
  280080:      	ldr	r0, [pc, #0x438]        @ 0x2804c0 <MMI_reset_00280040+0x480>
  280084:      	ldr	r1, [r0]
  280088:      	cmp	r1, #0
  28008c:      	bne	0x280088 <MMI_reset_00280040+0x48> @ imm = #-0xc
  280090:      	ldr	r1, [pc, #0xc8]         @ 0x280160 <MMI_reset_00280040+0x120>
  280094:      	str	r1, [r0]
  280098:      	mov	r0, #0
  28009c:      	ldr	r1, [pc, #0x418]        @ 0x2804bc <MMI_reset_00280040+0x47c>
  2800a0:      	cmp	r7, #19
  2800a4:      	beq	0x2800b0 <MMI_reset_00280040+0x70> @ imm = #0x4
  2800a8:      	ldr	r2, [r1, #0x28]
  2800ac:      	str	r2, [r0, #0x28]
  2800b0:      	ldr	r2, [r1, #0x18]
  2800b4:      	str	r2, [r0, #0x18]
  2800b8:      	ldr	r2, [r1, #0x38]
  2800bc:      	str	r2, [r0, #0x38]
  2800c0:      	ldr	r2, [r1, #0x1c]
  2800c4:      	str	r2, [r0, #0x1c]
  2800c8:      	ldr	r2, [r1, #0x3c]
  2800cc:      	str	r2, [r0, #0x3c]
  2800d0:      	ldr	r2, [r1, #0xc]
  2800d4:      	str	r2, [r0, #0xc]
  2800d8:      	ldr	r2, [r1, #0x2c]
  2800dc:      	str	r2, [r0, #0x2c]
  2800e0:      	ldr	r2, [r1, #0x10]
  2800e4:      	str	r2, [r0, #0x10]
  2800e8:      	ldr	r2, [r1, #0x30]
  2800ec:      	str	r2, [r0, #0x30]
  2800f0:      	ldr	sp, [pc, #0x394]        @ 0x28048c <MMI_reset_00280040+0x44c>
  2800f4:      	mov	r0, #210
  2800f8:      	msr	CPSR_fc, r0
  2800fc:      	ldr	sp, [pc, #0x38c]        @ 0x280490 <MMI_reset_00280040+0x450>
  280100:      	mov	r0, #219
  280104:      	msr	CPSR_fc, r0
  280108:      	ldr	sp, [pc, #0x380]        @ 0x280490 <MMI_reset_00280040+0x450>
  28010c:      	mov	r0, #211
  280110:      	msr	CPSR_fc, r0
  280114:      	msr	SPSR_fc, r0
  280118:      	ldr	sp, [pc, #0x3ac]        @ 0x2804cc <MMI_reset_00280040+0x48c>
  28011c:      	ldr	r1, [pc, #0x374]        @ 0x280498 <MMI_reset_00280040+0x458>
  280120:      	ldr	r2, [pc, #0x374]        @ 0x28049c <MMI_reset_00280040+0x45c>
  280124:      	mov	r0, #0
  280128:      	cmp	r1, r2
  28012c:      	beq	0x28013c <MMI_reset_00280040+0xfc> @ imm = #0x8
  280130:      	str	r0, [r1], #4
  280134:      	cmp	r1, r2
  280138:      	bls	0x280130 <MMI_reset_00280040+0xf0> @ imm = #-0x10
  28013c:      	bl	0x299c90 <MMI_diag_formatter_00298B68+0x1128> @ imm = #0x19b4c
  280140:      	bl	0x2976d0 <MMI_reset_00280040+0x17690> @ imm = #0x17588
  280144:      	bl	0x299a98 <MMI_diag_formatter_00298B68+0xf30> @ imm = #0x1994c
  280148:      	ldr	r1, [pc, #0x3a0]        @ 0x2804f0 <MMI_reset_00280040+0x4b0>
  28014c:      	mov	sp, r1
  280150:      	bl	0x297ce8 <MMI_reset_00280040+0x17ca8> @ imm = #0x17b90
  280154:      	b	0x280154 <MMI_reset_00280040+0x114> @ imm = #-0x8
  280158:      	mov	r0, #0
  28015c:      	mov	pc, r0
[MMI: native BIN header]

/mnt/data/spmp_stage53/work/SPMP-OS_Stage52_Ghidra_Imports/elf/MMI/ghidra_analysis_ONLY.elf:	file format elf32-littlearm

Disassembly of section .spmp_seg_0:

004611bc <MMI_native_NGame_header_verify_004611BC>:
  4611bc:      	mov	r12, sp
  4611c0:      	push	{r4, r5, r6, r7, r8, r9, r10, r11, r12, lr, pc}
  4611c4:      	sub	r11, r12, #4
  4611c8:      	sub	sp, sp, #12
  4611cc:      	mov	r5, r0
  4611d0:      	mov	r8, r1
  4611d4:      	mov	r10, r2
  4611d8:      	mov	r7, r3
  4611dc:      	ldr	r9, [r11, #0x4]
  4611e0:      	sub	r4, r11, #52
  4611e4:      	mov	r0, r4
  4611e8:      	mov	r1, r5
  4611ec:      	mov	r2, #5
  4611f0:      	bl	0x298c6c <MMI_diag_formatter_00298B68+0x104> @ imm = #-0x1c858c
  4611f4:      	mov	r6, #0
  4611f8:      	strb	r6, [r11, #-0x2f]
  4611fc:      	mov	r0, r4
  461200:      	ldr	r1, [pc, #0x58]         @ 0x461260 <MMI_native_NGame_header_verify_004611BC+0xa4>
  461204:      	bl	0x345f54 <MMI_SD_send_command_002B9B78+0x8c3dc> @ imm = #-0x11b2b8
  461208:      	cmp	r0, r6
  46120c:      	strne	r8, [r10]
  461210:      	strne	r6, [r9]
  461214:      	strne	r6, [r7]
  461218:      	movne	r0, #6
  46121c:      	bne	0x461258 <MMI_native_NGame_header_verify_004611BC+0x9c> @ imm = #0x34
  461220:      	ldr	r0, [pc, #0x3c]         @ 0x461264 <MMI_native_NGame_header_verify_004611BC+0xa8>
  461224:      	ldrb	r1, [r5, #0x6d]
  461228:      	bl	0x298b64 <MMI_diag_stub_00298B64> @ imm = #-0x1c86cc
  46122c:      	mov	r3, #128
  461230:      	str	r3, [r7]
  461234:      	ldr	r3, [r5, #0x78]
  461238:      	str	r3, [r9]
  46123c:      	sub	r2, r8, #128
  461240:      	rsb	r3, r3, r2
  461244:      	str	r3, [r10]
  461248:      	mov	r0, r5
  46124c:      	add	r1, r5, #128
  461250:      	bl	0x298c6c <MMI_diag_formatter_00298B68+0x104> @ imm = #-0x1c85ec
  461254:      	mov	r0, #0
  461258:      	sub	sp, r11, #40
  46125c:      	ldm	sp, {r4, r5, r6, r7, r8, r9, r10, r11, sp, pc}
  461260:      	subseq	r10, r0, r4, lsl lr
  461264:      	subseq	r10, r0, r12, lsl lr
[MMI: native BIN footer]

/mnt/data/spmp_stage53/work/SPMP-OS_Stage52_Ghidra_Imports/elf/MMI/ghidra_analysis_ONLY.elf:	file format elf32-littlearm

Disassembly of section .spmp_seg_0:

004611bc <MMI_native_NGame_header_verify_004611BC>:
  4618b0:      	add	r4, r6, r4
  4618b4:      	sub	r4, r4, #16
  4618b8:      	ldr	r0, [pc, #0x40]         @ 0x461900 <MMI_native_footer_verify_004618C4+0x3c>
  4618bc:      	mov	r1, r4
  4618c0:      	bl	0x298b64 <MMI_diag_stub_00298B64> @ imm = #-0x1c8d64

004618c4 <MMI_native_footer_verify_004618C4>:
  4618c4:      	mov	r0, r4
  4618c8:      	ldr	r1, [pc, #0x34]         @ 0x461904 <MMI_native_footer_verify_004618C4+0x40>
  4618cc:      	bl	0x345f54 <MMI_SD_send_command_002B9B78+0x8c3dc> @ imm = #-0x11b980
  4618d0:      	cmp	r0, #0
  4618d4:      	beq	0x4618e4 <MMI_native_footer_verify_004618C4+0x20> @ imm = #0x8
  4618d8:      	ldr	r0, [pc, #0x28]         @ 0x461908 <MMI_native_footer_verify_004618C4+0x44>
  4618dc:      	bl	0x298b64 <MMI_diag_stub_00298B64> @ imm = #-0x1c8d80
  4618e0:      	mov	r5, #5
  4618e4:      	mov	r0, r5
  4618e8:      	sub	sp, r11, #24
  4618ec:      	ldm	sp, {r4, r5, r6, r11, sp, pc}
[MMI: built-in game handoff]

/mnt/data/spmp_stage53/work/SPMP-OS_Stage52_Ghidra_Imports/elf/MMI/ghidra_analysis_ONLY.elf:	file format elf32-littlearm

Disassembly of section .spmp_seg_0:

0046018c <MMI_N1_loader_0046018C>:
  4601e0:      	ldr	r4, [pc, #0x88]         @ 0x460270 <MMI_N1_loader_0046018C+0xe4>
  4601e4:      	mov	r3, #10682368
  4601e8:      	add	r3, r3, #16384
  4601ec:      	str	r3, [r4, #0x8]
  4601f0:      	add	r3, r3, #1032192
  4601f4:      	add	r3, r3, #16384
  4601f8:      	str	r3, [r4, #0x10]
  4601fc:      	ldr	r3, [pc, #0x68]         @ 0x46026c <MMI_N1_loader_0046018C+0xe0>
  460200:      	ldr	r3, [r3]
  460204:      	sub	r3, r3, #1261568
  460208:      	str	r3, [r4, #0x14]
  46020c:      	ldr	r2, [r11, #-0x7c]
  460210:      	str	r2, [r4, #0xc]
  460214:      	str	r5, [r4, #0x4]
  460218:      	mov	r3, #1
  46021c:      	str	r3, [r4, #0x18]
  460220:      	str	r2, [sp]
  460224:      	ldr	r3, [pc, #0x48]         @ 0x460274 <MMI_N1_loader_0046018C+0xe8>
  460228:      	ldr	r0, [r3]
  46022c:      	mov	r1, #10485760
  460230:      	sub	r2, r11, #120
  460234:      	mov	r3, #212992
  460238:      	bl	0x464158 <MMI_native_footer_verify_004618C4+0x2894> @ imm = #0x3f18
  46023c:      	strb	r0, [r4, #0x1]
  460240:      	mov	r0, r4
  460244:      	ldr	r1, [pc, #0x2c]         @ 0x460278 <MMI_N1_loader_0046018C+0xec>
  460248:      	ldr	r2, [pc, #0x2c]         @ 0x46027c <MMI_N1_loader_0046018C+0xf0>
  46024c:      	mov	r3, #10485760
  460250:      	mov	lr, pc
  460254:      	mov	pc, r3
  460258:      	sub	sp, r11, #20
  46025c:      	ldm	sp, {r4, r5, r11, sp, pc}
[MMI: diag handler]

/mnt/data/spmp_stage53/work/SPMP-OS_Stage52_Ghidra_Imports/elf/MMI/ghidra_analysis_ONLY.elf:	file format elf32-littlearm

Disassembly of section .spmp_seg_0:

00298b64 <MMI_diag_stub_00298B64>:
  298b64:      	<unknown>

00298b68 <MMI_diag_formatter_00298B68>:
  298b68:      	push	{r0, r1, r2, r3}
  298b6c:      	push	{r4, r5, r11, r12, lr, pc}
  298b70:      	sub	r11, r12, #20
  298b74:      	ldr	r5, [r11, #0x4]
  298b78:      	ldr	r3, [pc, #0x4c]         @ 0x298bcc <MMI_diag_formatter_00298B68+0x64>
  298b7c:      	ldr	r3, [r3]
  298b80:      	cmp	r3, #0
  298b84:      	moveq	r0, #1
  298b88:      	beq	0x298bc8 <MMI_diag_formatter_00298B68+0x60> @ imm = #0x38
  298b8c:      	add	r4, r11, #8
  298b90:      	ldr	r0, [pc, #0x38]         @ 0x298bd0 <MMI_diag_formatter_00298B68+0x68>
  298b94:      	mov	r1, r5
  298b98:      	mov	r2, r4
  298b9c:      	bl	0x2989a0 <MMI_reset_00280040+0x18960> @ imm = #-0x204
  298ba0:      	mov	r2, r0
  298ba4:      	ldr	r0, [pc, #0x28]         @ 0x298bd4 <MMI_diag_formatter_00298B68+0x6c>
  298ba8:      	ldr	r1, [pc, #0x20]         @ 0x298bd0 <MMI_diag_formatter_00298B68+0x68>
  298bac:      	bl	0x298a6c <MMI_reset_00280040+0x18a2c> @ imm = #-0x148
  298bb0:      	ldr	r3, [pc, #0x20]         @ 0x298bd8 <MMI_diag_formatter_00298B68+0x70>
  298bb4:      	ldr	r0, [r3]
  298bb8:      	mov	r1, #0
  298bbc:      	mov	r2, r5
  298bc0:      	mov	r3, r4
  298bc4:      	bl	0x29804c <MMI_reset_00280040+0x1800c> @ imm = #-0xb80
  298bc8:      	ldm	sp, {r4, r5, r11, sp, pc}
[RedBoot family1 factory]

/mnt/data/spmp_stage53/work/SPMP-OS_Stage52_Ghidra_Imports/elf/RedBoot_variant_1/ghidra_analysis_ONLY.elf:	file format elf32-littlearm

Disassembly of section .spmp_seg_2:

0004a998 <RedBoot_factory_start_0004A998>:
   4a998:      	push	{r4, r5, r6, r11, r12, lr, pc}
   4a99c:      	sub	r11, r12, #4
   4a9a0:      	sub	sp, sp, #44
   4a9a4:      	mov	r5, #0
   4a9a8:      	ldr	r0, [pc, #0x8c]         @ 0x4aa3c <RedBoot_factory_start_0004A998+0xa4>
   4a9ac:      	bl	0x49e4c <RedBoot_ROMFS_loader_00038F0C+0x10f40> @ imm = #-0xb68
   4a9b0:      	mov	r6, #1
   4a9b4:      	ldr	r3, [pc, #0x84]         @ 0x4aa40 <RedBoot_factory_start_0004A998+0xa8>
   4a9b8:      	str	r6, [r3]
   4a9bc:      	ldr	r0, [pc, #0x80]         @ 0x4aa44 <RedBoot_factory_start_0004A998+0xac>
   4a9c0:      	ldr	r1, [pc, #0x80]         @ 0x4aa48 <RedBoot_factory_start_0004A998+0xb0>
   4a9c4:      	bl	0x38f0c <RedBoot_ROMFS_loader_00038F0C> @ imm = #-0x11ac0
   4a9c8:      	subs	r4, r0, #0
   4a9cc:      	beq	0x4aa14 <RedBoot_factory_start_0004A998+0x7c> @ imm = #0x40
   4a9d0:      	ldr	r3, [pc, #0x74]         @ 0x4aa4c <RedBoot_factory_start_0004A998+0xb4>
   4a9d4:      	str	r3, [r11, #-0x44]
   4a9d8:      	ldr	r3, [pc, #0x70]         @ 0x4aa50 <RedBoot_factory_start_0004A998+0xb8>
   4a9dc:      	str	r3, [r11, #-0x40]
   4a9e0:      	sub	r0, r11, #56
   4a9e4:      	str	r0, [r11, #-0x3c]
   4a9e8:      	ldr	r1, [pc, #0x64]         @ 0x4aa54 <RedBoot_factory_start_0004A998+0xbc>
   4a9ec:      	mov	r2, r4
   4a9f0:      	bl	0x49cd4 <RedBoot_ROMFS_loader_00038F0C+0x10dc8> @ imm = #-0xd24
   4a9f4:      	ldr	r0, [pc, #0x5c]         @ 0x4aa58 <RedBoot_factory_start_0004A998+0xc0>
   4a9f8:      	mov	r1, r4
   4a9fc:      	bl	0x49e4c <RedBoot_ROMFS_loader_00038F0C+0x10f40> @ imm = #-0xbb8
   4aa00:      	mov	r0, #3
   4aa04:      	sub	r1, r11, #68
   4aa08:      	bl	0x4b304 <RedBoot_factory_start_0004A998+0x96c> @ imm = #0x8f4
   4aa0c:      	mov	r5, r6
   4aa10:      	b	0x4aa28 <RedBoot_factory_start_0004A998+0x90> @ imm = #0x10
   4aa14:      	ldr	r0, [pc, #0x40]         @ 0x4aa5c <RedBoot_factory_start_0004A998+0xc4>
   4aa18:      	bl	0x49e4c <RedBoot_ROMFS_loader_00038F0C+0x10f40> @ imm = #-0xbd4
   4aa1c:      	mov	r2, #0
   4aa20:      	ldr	r3, [pc, #0x18]         @ 0x4aa40 <RedBoot_factory_start_0004A998+0xa8>
   4aa24:      	str	r2, [r3]
   4aa28:      	ldr	r0, [pc, #0x30]         @ 0x4aa60 <RedBoot_factory_start_0004A998+0xc8>
   4aa2c:      	bl	0x49e4c <RedBoot_ROMFS_loader_00038F0C+0x10f40> @ imm = #-0xbe8
   4aa30:      	mov	r0, r5
   4aa34:      	sub	sp, r11, #24
   4aa38:      	ldm	sp, {r4, r5, r6, r11, sp, pc}
   4aa3c:      	andeq	r9, r5, r4, lsr #23
[RedBoot family3 factory]

/mnt/data/spmp_stage53/work/SPMP-OS_Stage52_Ghidra_Imports/elf/RedBoot_variant_3/ghidra_analysis_ONLY.elf:	file format elf32-littlearm

Disassembly of section .spmp_seg_2:

000467f4 <RedBoot_factory_start_000467F4>:
   467f4:      	push	{r4, r5, lr}
   467f8:      	sub	sp, sp, #44
   467fc:      	ldr	r0, [pc, #0x90]         @ 0x46894 <RedBoot_factory_start_000467F4+0xa0>
   46800:      	bl	0x4563c <RedBoot_ROMFS_loader_00036BF4+0xea48> @ imm = #-0x11cc
   46804:      	mov	r5, #1
   46808:      	ldr	r3, [pc, #0x88]         @ 0x46898 <RedBoot_factory_start_000467F4+0xa4>
   4680c:      	str	r5, [r3]
   46810:      	ldr	r0, [pc, #0x84]         @ 0x4689c <RedBoot_factory_start_000467F4+0xa8>
   46814:      	ldr	r1, [pc, #0x84]         @ 0x468a0 <RedBoot_factory_start_000467F4+0xac>
   46818:      	bl	0x36bf4 <RedBoot_ROMFS_loader_00036BF4> @ imm = #-0xfc2c
   4681c:      	subs	r4, r0, #0
   46820:      	beq	0x46868 <RedBoot_factory_start_000467F4+0x74> @ imm = #0x40
   46824:      	ldr	r3, [pc, #0x78]         @ 0x468a4 <RedBoot_factory_start_000467F4+0xb0>
   46828:      	str	r3, [sp, #0x20]
   4682c:      	ldr	r3, [pc, #0x74]         @ 0x468a8 <RedBoot_factory_start_000467F4+0xb4>
   46830:      	str	r3, [sp, #0x24]
   46834:      	mov	r0, sp
   46838:      	str	sp, [sp, #0x28]
   4683c:      	ldr	r1, [pc, #0x68]         @ 0x468ac <RedBoot_factory_start_000467F4+0xb8>
   46840:      	mov	r2, r4
   46844:      	bl	0x45890 <RedBoot_ROMFS_loader_00036BF4+0xec9c> @ imm = #-0xfbc
   46848:      	ldr	r0, [pc, #0x60]         @ 0x468b0 <RedBoot_factory_start_000467F4+0xbc>
   4684c:      	mov	r1, r4
   46850:      	bl	0x4563c <RedBoot_ROMFS_loader_00036BF4+0xea48> @ imm = #-0x121c
   46854:      	mov	r0, #3
   46858:      	add	r1, sp, #32
   4685c:      	bl	0x46220 <RedBoot_ROMFS_loader_00036BF4+0xf62c> @ imm = #-0x644
   46860:      	mov	r4, r5
   46864:      	b	0x46880 <RedBoot_factory_start_000467F4+0x8c> @ imm = #0x14
   46868:      	ldr	r0, [pc, #0x44]         @ 0x468b4 <RedBoot_factory_start_000467F4+0xc0>
   4686c:      	bl	0x4563c <RedBoot_ROMFS_loader_00036BF4+0xea48> @ imm = #-0x1238
   46870:      	mov	r2, #0
   46874:      	ldr	r3, [pc, #0x1c]         @ 0x46898 <RedBoot_factory_start_000467F4+0xa4>
   46878:      	str	r2, [r3]
   4687c:      	mov	r4, r2
   46880:      	ldr	r0, [pc, #0x30]         @ 0x468b8 <RedBoot_factory_start_000467F4+0xc4>
   46884:      	bl	0x4563c <RedBoot_ROMFS_loader_00036BF4+0xea48> @ imm = #-0x1250
   46888:      	mov	r0, r4
   4688c:      	add	sp, sp, #44
   46890:      	pop	{r4, r5, pc}
