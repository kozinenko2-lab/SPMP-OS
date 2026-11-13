#include "redboot_reloc_model.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
    uint32_t out;
    const uint32_t types[]={1,28,29};
    for(unsigned i=0;i<3;i++) {
        assert(rb_relocate_word_model(types[i],0x1000,0x2000,0xebfffffe,&out)==0);
        assert(out==0xeb0003fe); /* ARM PC+8 + sign-extended imm24*4 = 0x2000. */
        assert(rb_relocate_word_model(types[i],0x2000,0x1000,0xeafffffe,&out)==0);
        assert(out==0xeafffbfe);
        /* A far symbol wraps like the OEM implementation; it is not validated. */
        assert(rb_relocate_word_model(types[i],0x1000,0x04002000,0xebfffffe,&out)==0);
        assert(out==0xeb0003fe);
    }
    assert(rb_relocate_word_model(2,0,0xfffffff0u,0x30,&out)==0 && out==0x20);
    for(uint32_t t=0;t<256;t++) {
        if(t==1 || t==2 || t==28 || t==29) continue;
        assert(rb_relocate_word_model(t,0x1000,0x2000,0x12345678,&out)==-1);
        assert(out==0x12345678);
    }
    puts("PASS: OEM relocation arithmetic, backward branches, overflow and unsupported types");
    return 0;
}
