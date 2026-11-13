
research/boot_reconstruction/build/native_startup/application.elf:	file format elf32-littlearm

Disassembly of section .text:

00a00000 <_spmp_start>:
  a00000: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
  a00004: e1a0400d     	mov	r4, sp
  a00008: e3cdd007     	bic	sp, sp, #7
  a0000c: e1a05000     	mov	r5, r0
  a00010: e59f1024     	ldr	r1, [pc, #0x24]         @ 0xa0003c <_spmp_start+0x3c>
  a00014: e59f2024     	ldr	r2, [pc, #0x24]         @ 0xa00040 <_spmp_start+0x40>
  a00018: e3a03000     	mov	r3, #0
  a0001c: e1510002     	cmp	r1, r2
  a00020: 34813004     	strlo	r3, [r1], #4
  a00024: 3afffffc     	blo	0xa0001c <_spmp_start+0x1c> @ imm = #-0x10
  a00028: e1a00005     	mov	r0, r5
  a0002c: eb000004     	bl	0xa00044 <spmp_app_main> @ imm = #0x10
  a00030: e1a0d004     	mov	sp, r4
  a00034: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
  a00038: e12fff1e     	bx	lr
