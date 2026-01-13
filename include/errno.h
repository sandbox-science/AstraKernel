/**
* @file errno.h
* @brief Error code for kernel
*
* This module provide multiple error code for the kernel.
*/
#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @enum kerror_t
 * @brief Enumerator for kernel error code.
 *
 * This enumerator define multiple error code for kernel,
 * including KERR_OK with value 0 for no error.
 */
typedef enum kerror_code : int32_t
{
    KERR_OK        =  0,  /**< code  0 if ok **/ 
    KERR_NOT_FOUND = -1,  /**< code -1 if not found**/
    KERR_NOMEM     = -2,  /**< code -2 if out of memory**/
    KERR_NO_SPACE  = -3,  /**< code -3 if out of space**/
    KERR_INVAL     = -4,  /**< code -4 if invalid request**/
} kerror_t;

/**
 * @brief Helper function for no error kernel code.
 *
 * Check if the kernel error code is equal to 0,
 * meaning no error detected.
 */
static inline bool kerr_is_ok(kerror_t e)
{
    return e == 0;
}

/**
 * @brief Helper function for error kernel code.
 *
 * Check if the kernel error code is less than 0,
 * meaning an error occured.
 */
static inline bool kerr_is_err(kerror_t e)
{
    return e < 0;
}

/**
 * @brief Formatted string for kernel error.
 *
 * Provide a formatted string based on the kernel error code.
 */
const char* error_str(kerror_t error_code);
