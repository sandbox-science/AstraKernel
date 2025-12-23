/**
 * @file printf.h
 * @brief Minimal kernel printf/puts/getlines interface for freestanding systems.
 *
 * This header defines the kernel's text I/O functions and the
 * internal formatting structures used by the printf subsystem.
 * 
 * The implementation targets bare-metal systems using memory-mapped UART output,
 * and is fully freestanding (no libc dependencies).
 */
#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @enum fmt_state_t
     * @brief Finite-state machine for parsing format strings.
     * 
     * The printf parser moves between these states as it consumes characters.
    */
    typedef enum fmt_state : int32_t
    {
        FMT_TEXT,    /**< Normal character output. */
        FMT_PERCENT, /**< After encountering '\%'. */
        FMT_LONG     /**< After encountering '\%l'. */
    } fmt_state_t;

    /**
     * @enum fmt_flag_t
     * @brief Formatting flags for integer conversions.
     *
     * Used internally by the printf implementation.
    */
    typedef enum fmt_flag : uint32_t
    {
        FLAG_LONG      = 1u << 0, /**< 'l' length modifier (long / long long). */
        FLAG_UNSIGNED  = 1u << 1, /**< Future use: unsigned type hint. */
        FLAG_HEX       = 1u << 2, /**< Future use: hexadecimal output flag. */
        FLAG_UPPERCASE = 1u << 3  /**< Uppercase hex digits (for %X). */
    } fmt_flag_t;

    /**
     * @struct fmt_args_t
     * @brief Integer conversion arguments for unsigned formats (%u, %x, %X).

     * @details Holds the numeric value and formatting flags for integer output.
     * Used internally by the printf implementation.
    */
    typedef struct
    {
        unsigned long long num;   /**< Unsigned numeric value to format. */
        fmt_flag_t         flags; /**< Formatting flags (length, case, etc.). */
    } fmt_args_t;

    /**
     * @brief Transmit a null-terminated string over UART.
     *
     * @param s The string to output. If NULL, no output occurs.
    */
    void puts(const char *s);

    /**
     * @brief Prints a formatted string to the UART output.
     *
     * @param fmt Format string (supports %c, %s, %d, %u, %x, %X, %p, %%).
     * @param ... Variable arguments matching the format specifiers.
     *
     * This function supports a minimal subset of standard C printf:
     * - Signed/unsigned integers (`%d`, `%u`)
     * - Hexadecimal (`%x`, `%X`)
     * - Pointers (`%p`)
     * - Characters (`%c`)
     * - Strings (`%s`)
     * - Length modifier (`%l`)
    */
    void printf(const char *fmt, ...);

    /**
     * @brief Reads a line of text from UART into the given buffer.
     *
     * @param buffer Destination buffer.
     * @param length Maximum buffer length (including null terminator).
     *
     * @note Blocks until a newline or carriage return is received.
     * Supports backspace editing and echoes input characters.
    */
    void getlines(char *restrict buffer, size_t length);

#ifdef __cplusplus
}
#endif
