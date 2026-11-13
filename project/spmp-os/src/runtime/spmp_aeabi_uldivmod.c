/* Portable unsigned 64-bit long division for ARMv5 soft-float runtime.
 * No native / or % operators: avoids compiler runtime recursion.
 * Division by zero is an invalid caller request; this helper reports 0 and
 * retains the dividend as remainder, rather than trapping in firmware.
 */
#include <stdint.h>
uint64_t spmp_udivmod64(uint64_t numerator, uint64_t denominator, uint64_t *remainder) {
    if (denominator == 0) {
        if (remainder) *remainder = numerator;
        return 0;
    }
    uint64_t q = 0, r = 0;
    for (unsigned i = 0; i < 64; ++i) {
        const unsigned carry = (unsigned)(r >> 63);
        r = (r << 1) | (numerator >> 63);
        numerator <<= 1;
        q <<= 1;
        if (carry || r >= denominator) {
            r -= denominator;
            q |= 1;
        }
    }
    if (remainder) *remainder = r;
    return q;
}
