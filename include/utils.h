/**
 * @file utils.h
 * @brief Common macros and helpers.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Align a pointer up to the next multiple of a given alignment.
 *
 * This function takes a pointer value `x` and aligns it up to the next
 * multiple of `align`. The `align` parameter must be a power of 2.
 *
 * @param x The pointer value to align.
 * @param align The alignment value (must be a power of 2).
 *
 * @return uintptr_t The aligned pointer value.
 */
static inline uintptr_t align_up_uintptr(uintptr_t x, size_t align)
{
    return (x + (align - 1)) & ~(uintptr_t)(align - 1);
}
