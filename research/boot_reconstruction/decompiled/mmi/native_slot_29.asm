002de618 cpy r12,sp
002de61c stmdb sp!,{r4,r11,r12,lr,pc}
002de620 sub r11,r12,#0x4
002de624 and r4,r0,#0xff
002de628 and r1,r1,#0xff
002de62c add r2,r4,#0x10
002de630 and r2,r2,#0xff
002de634 mov r3,r1, lsl #0x18
002de638 mov r0,r3, asr #0x18
002de63c cmn r0,#0x10
002de640 movge r3,#0x0
002de644 movlt r3,#0x1
002de648 cmp r2,#0x1f
002de64c orrhi r3,r3,#0x1
002de650 cmp r3,#0x0
002de654 bne 0x002de660
002de658 cmp r0,#0xf
002de65c ble 0x002de66c
002de660 mov r0,#0x400
002de664 add r0,r0,#0x70000001
002de668 ldmia sp,{r4,r11,sp,pc}
002de66c mov r1,r1, lsl #0x18
002de670 ldr r0,[0x2de694]
002de674 mov r1,r1, asr #0x18
002de678 bl 0x00312d28
002de67c mov r1,r4, lsl #0x18
002de680 ldr r0,[0x2de698]
002de684 mov r1,r1, asr #0x18
002de688 bl 0x00312d28
002de68c mov r0,#0x0
002de690 ldmia sp,{r4,r11,sp,pc}
REFERENCES TO ENTRY:
002d33d8 DATA
