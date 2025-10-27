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
    typedef enum {
        FMT_TEXT,    /**< Normal character output. */
        FMT_PERCENT, /**< After encountering '\%'. */
        FMT_LONG     /**< After encountering '\%l'. */
    } fmt_state_t;

    /**
    * @brief Bit flags controlling number formatting.
    */
    enum {
        FLAG_LONG      = 1 << 0, /**< 'l' length modifier (long / long long). */
        FLAG_UNSIGNED  = 1 << 1, /**< Future use: unsigned type hint. */
        FLAG_HEX       = 1 << 2, /**< Future use: hexadecimal output flag. */
        FLAG_UPPERCASE = 1 << 3  /**< Uppercase hex digits (for %X). */
    };

    /**
    * @struct Format_State
    * @brief Stores the current numeric value and formatting flags.
    *
    * This structure is passed to integer-formatting functions during printf
    * processing. It represents the transient state for one format specifier.
    */
    typedef struct {
        unsigned long long num; /**< The numeric value to be printed. */
        uint8_t flags;          /**< Bitmask of FLAG_* constants describing format. */
    } Format_State;

    /**
    * @brief Prints a null-terminated string over UART.
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
    * Blocks until a newline or carriage return is received.
    * Supports backspace editing and echoes input characters.
    */
    void getlines(char *restrict buffer, size_t length);

#ifdef __cplusplus
}
#endif
