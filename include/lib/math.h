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
        kernel_panic("Division by zero in udiv32.", KERR_INVAL);
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

/**
 * @brief Perform division and return quotient and remainder.
 *
 * @note Kernel panic if division by zero
 * 
 * @param n Dividend.
 * @param d Divisor.
 * @param r Output parameter for remainder.
 * @return Quotient of n / d.
 */
static inline unsigned long long _divmod(unsigned long long n, unsigned long long d, unsigned long long *r)
{
    if (d == 0)
    {
        kernel_panic("_divmod: Division by zero", KERR_INVAL);
    }

    *r = n % d;
    return n / d;
}

/**
 * @brief Perform binary long division on unsigned long long integers.
 *
 * @param dividend The number to be divided.
 * @param divisor The number to divide by.
 * @param remainder Pointer to store the remainder of the division.
 * @return The quotient of the division.
 *
 * @note Kernel panic if division by zero.
 */
// unsigned long long _bdiv(unsigned long long dividend, unsigned long long divisor, unsigned long long *remainder)
// {
//     // TO-DO: Design a faster division algorithm
//     if (divisor == 0)
//     {
//         kernel_panic("_bdiv: Division by zero", KERR_INVAL);
//     }
//     *remainder = 0;
//     unsigned long long quotient = 0;

//     for (int i = 63; i >= 0; i--)
//     {
//         quotient <<= 1;
//         *remainder <<= 1;
//         unsigned long long temp = (unsigned long long)1 << i; // Without this cast, the type is misinterpreted leading to UB
//         *remainder |= (dividend & temp) >> i;

//         if (*remainder >= divisor)
//         {
//             *remainder -= divisor;
//             quotient |= 1;
//         }
//     }

//     return quotient;
// }