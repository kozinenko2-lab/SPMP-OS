004ae52c ldr r11,[r0,#0x0]
004ae530 mrs r0,cpsr
004ae534 orr r0,r0,#0xc0
004ae538 msr cpsr_cf,r0
004ae53c ldr r0,[r11,#0x40]
004ae540 msr spsr_cf,r0
004ae544 ldmia r11,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,sp,lr}
004ae548 movs pc,lr
REFERENCES TO ENTRY:
0033c144 UNCONDITIONAL_CALL
