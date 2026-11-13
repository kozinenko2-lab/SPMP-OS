/* Host-only arithmetic reconstruction, RedBoot 3/5 at 0x413D0.
 * Not a complete ELF loader. Faithfully retains OEM lack of range/alignment checks.
 */
#include "redboot_reloc_model.h"
int rb_relocate_word_model(uint32_t type,uint32_t place,uint32_t symbol,
                           uint32_t old_word,uint32_t *new_word)
{
    *new_word=old_word;
    switch(type) {
    case 1:  /* R_ARM_PC24 */
    case 28: /* R_ARM_CALL */
    case 29: /* R_ARM_JUMP24 */ {
        uint32_t addend=(old_word & 0x00ffffffu)<<2;
        if(addend & 0x02000000u) addend |= 0xfc000000u;
        uint32_t delta=symbol-place+addend;
        *new_word=(old_word & 0xff000000u) | ((delta & 0x03ffffffu)>>2);
        return 0;
    }
    case 2: /* R_ARM_ABS32 */
        *new_word=old_word+symbol;
        return 0;
    default:return -1;
    }
}
