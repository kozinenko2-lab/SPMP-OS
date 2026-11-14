#ifndef SPMP_ALIAS_TEST_H
#define SPMP_ALIAS_TEST_H
#include "input_test.h"

#define SPMP_ALIAS_BIT 0x10000000u
#define SPMP_ALIAS_WORDS 8u

enum alias_test_result {
    ALIAS_TEST_PASS=0,
    ALIAS_TEST_BAD_SOURCE=1,
    ALIAS_TEST_MISMATCH=2,
    ALIAS_TEST_GRAPHICS_ERROR=3
};

struct alias_test_record {
    uint32_t source;
    uint32_t alias;
    uint32_t result;
    uint32_t mismatch_mask;
    uint32_t observed[SPMP_ALIAS_WORDS];
    uint32_t log_status;
    uint32_t log_errno;
};

extern const uint32_t spmp_alias_seed[SPMP_ALIAS_WORDS];
extern struct alias_test_record spmp_alias_record;
uint32_t alias_test_run(const struct input_services *io);
#endif
