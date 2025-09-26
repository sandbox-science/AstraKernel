#pragma once

#include <stdint.h>
#include "panic.h"

/**
 * @brief Unsigned 32-bit integer division: n / d
 *
 * @param n Numerator.
 * @param d Denominator.
 * 
 * @return uint32_t Quotient, discards remainder.
 *
 * @note If d == 0: kernel panic.
 *       Otherwise, returns the quotient and discards the remainder.
 */
static inline uint32_t _udiv32(uint32_t n, uint32_t d)
{
    if (d == 0)
    {
        kernel_panic("Error: Division by zero in udiv32.\n");
    }

    uint32_t quotient  = 0;
    uint32_t remainder = 0;
    for (int i = 31; i >= 0; --i)
    {
        remainder = (remainder << 1) | ((n >> i) & 1u);
        if (remainder >= d)
        {
            remainder -= d;
            quotient  |= (1u << i);
        }
    }
    return quotient;
}
// Note: This is a simple implementation and may not be the most efficient.
// It is intended for educational purposes and may be replaced with
// architecture-specific optimizations if needed.
