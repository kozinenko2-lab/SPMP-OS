#ifndef SPMP_RB_RELOC_MODEL_H
#define SPMP_RB_RELOC_MODEL_H
#include <stdint.h>
int rb_relocate_word_model(uint32_t type,uint32_t place,uint32_t symbol,
                           uint32_t old_word,uint32_t *new_word);
#endif
