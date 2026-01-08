/**
 * @file printf.c
 * @brief UART-backed minimal printf implementation for a freestanding kernel.
 *
 * This file implements formatted output and simple line input using
 * the QEMU VersatileAB UART0. It is designed for early boot and debugging.
 *
 * @todo Refactor this file for cleaner function implementation.
 */
#include "printf.h"
#include "panic.h"
#include "lib/math.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");

/** @todo: See to move this mathematical operation to math.c/.h */
// Maybe have a file with the macros?
// Memory-mapped I/O registers for UART0 on QEMU versatileAB
#define     UART0_BASE  0x101F1000
#define     UART0_DR    (*(volatile uint32_t *)UART0_BASE)          // Data Register
#define     UART0_FR    (*(volatile uint32_t *)(UART0_BASE + 0x18)) // Flag Register

static const uint32_t UART_FR_TXFF = (1U << 5); // Transmit FIFO Full
static const uint32_t UART_FR_RXFE = (1U << 4); // Receive FIFO Empty

/**
 * @brief Send a single character over UART, waiting if the FIFO is full
 *
 * @param c Character to transmit
*/
static inline void putc(char c)
{
    // Wait until UART transmit FIFO is not full
    while (UART0_FR & UART_FR_TXFF) {}
    UART0_DR = (uint32_t)c;
}

/**
 * @brief Print an unsigned integer in the given base.
 *
 * @param num Unsigned integer to print.
 * @param base Number base (10 for decimal, 16 for hex).
 * @param uppercase Use uppercase hex digits if true.
 */
static void put_unsigned(unsigned long long num, unsigned base, bool uppercase)
{
    static const char digits_l[] = "0123456789abcdef";
    static const char digits_u[] = "0123456789ABCDEF";
    const char *digits = uppercase ? digits_u : digits_l;

    char buf[32];
    size_t len = 0;

    do
    {
        unsigned long long rem;
        num         = _divmod(num, base, &rem);
        buf[len++]  = digits[rem];
    } while (num);

    while (len--)
    {
        putc(buf[len]);
    }
}

/**
 * @brief Print a signed integer in decimal form.
 *
 * @param num Signed integer to print.
 */
static inline void put_signed(long long num)
{
    if (num < 0)
    {
        putc('-');
        unsigned long long mag = (unsigned long long)(-(num + 1)) + 1;
        put_unsigned(mag, 10, false);
        return;
    }
    put_unsigned((unsigned long long)num, 10, false);
}

/**
 * @brief Format and print an unsigned integer based on the format specifier.
 *
 * Handles %u, %x, %X using the provided format state.
 *
 * @param control Format specifier character ('u', 'x', or 'X').
 * @param fs Pointer to the current format state.
 */
static inline void put_integers(char control, const fmt_args_t *restrict fs)
{
    bool uppercase           = fs->flags & FLAG_UPPERCASE;
    unsigned long long value = fs->num;

    switch (control)
    {
        case 'u': // unsigned decimal
            put_unsigned(value, 10, false);
            break;

        case 'x': // lowercase hex
        case 'X': // uppercase hex
            if (uppercase)
            {
                put_unsigned(value, 16, true);
            }
            else
            {
                put_unsigned(value, 16, false);
            }
            break;

        default:
            // Optional: add kernel diagnostic for unknown format
            break;
    }
}

void puts(const char *s)
{
    if (s == NULL)
    {
        return;
    }

    while (*s)
    {
        putc(*s++);
    }
}

void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fmt_state_t state   = FMT_TEXT;
    fmt_args_t  fs      = {0};

    while (*fmt)
    {
        char c = *fmt++;

        switch (state)
        {
            case FMT_TEXT:
            {
                if (c == '%')
                {
                    state = FMT_PERCENT;
                }
                else
                {
                    putc(c);
                }
                break;
            }

            case FMT_PERCENT:
            {
                switch (c)
                {
                    case '%':
                    {
                        putc('%');
                        state = FMT_TEXT;
                        break;
                    }

                    case 'l':
                    {
                        fs.flags |= FLAG_LONG;
                        state     = FMT_LONG;
                        break;
                    }

                    case 'c':
                    {
                        putc((char)va_arg(args, int));
                        state = FMT_TEXT;
                        break;
                    }

                    case 's':
                    {
                        const char *s = va_arg(args, const char *);
                        puts(s ? s : "(null)");
                        state = FMT_TEXT;
                        break;
                    }

                    case 'p':
                    {
                        uintptr_t addr = (uintptr_t)va_arg(args, void *);
                        puts("0x");
                        put_unsigned(addr, 16, false);
                        fs.flags = 0; // reset for next
                        state = FMT_TEXT;
                        break;
                    }

                    case 'd':
                    {
                        put_signed(va_arg(args, int));
                        fs.flags = 0; // reset for next
                        state = FMT_TEXT;
                        break;
                    }

                    case 'u':
                    case 'x':
                    case 'X':
                    {
                        fs.num = (unsigned long long)va_arg(args, unsigned int);
                        if (c == 'X')
                        {
                            fs.flags |= FLAG_UPPERCASE;
                        }

                        put_integers(c, &fs);
                        fs.flags = 0; // reset for next
                        state    = FMT_TEXT;
                        break;
                    }

                    default:
                    {
                        // unrecognized specifier
                        putc('%');
                        putc(c);
                        fs.flags = 0; // reset for next
                        state    = FMT_TEXT;
                        break;
                    }
                }
                break;
            }

            case FMT_LONG:
            {
                // only handles "%ld", "%lu", "%lx", "%lX" for now
                switch (c)
                {
                    case 'd':
                    {
                        put_signed(va_arg(args, long));
                        fs.flags = 0; // reset for next
                        state    = FMT_TEXT;
                        break;
                    }

                    case 'u':
                    case 'x':
                    case 'X':
                    {
                        fs.num = (unsigned long long)va_arg(args, unsigned long);
                        if (c == 'X')
                        {
                            fs.flags |= FLAG_UPPERCASE;
                        }

                        put_integers(c, &fs);
                        fs.flags = 0;
                        state    = FMT_TEXT;
                        break;
                    }

                    default:
                        // fallback if 'l' wasn't followed by known type
                        putc('%');
                        putc('l');
                        putc(c);
                        fs.flags = 0; // reset for next
                        state    = FMT_TEXT;
                        break;
                }
                break;
            }
        }
    }

    va_end(args);
}

/**
 * @brief Blocking UART character input.
 *
 * @return The next character read from UART.
 */
static inline char getc(void)
{
    // Wait until UART receive FIFO is not empty
    while (UART0_FR & UART_FR_RXFE) {}
    return (char)(UART0_DR & 0xFF);
}

void getlines(char *restrict buffer, size_t length)
{
    long index = 0;
    long cursor_position = 0;

    char character;

    uint8_t escape = 0;
    uint8_t arrow_keys = 0;

    while (index < (long)length - 1)
    {
        character = getc();

        if (character == '\033') // Handle Escape sequences
        {
            escape = 1;
            continue;
        }

        if (escape)
        {
            if (escape == 1)
            {
                arrow_keys = (character == 91);
            }
            else
            {
                if (arrow_keys)
                {
                    switch (character)
                    {
                    case 'A': // Up
                    {
                        break;
                    }
                    case 'B': // Down
                    {
                        break;
                    }
                    case 'C': // Right
                    {
                        if (cursor_position < index)
                        {
                            puts("\033[C");
                            cursor_position++;
                        }
                        break;
                    }
                    case 'D': // Left
                    {
                        if (cursor_position - 1 >= 0)
                        {
                            puts("\033[D");
                            cursor_position--;
                        }
                    }
                    default:
                    {
                        break;
                    }
                    }

                    arrow_keys = 0;
                }
            }
            escape++;

            if (escape == 3) // Escape sequence is 3 characters long
            {
                escape = 0;
            }
            continue;
        }

        if (character == '\r') // Check for carriage return
        {
            break;
        }
        if (character == '\b' || character == 0x7F) // Check for backspace
        {
            if (cursor_position > 0 && index > 0) // Delete char if present
            {
                long initial_pos = cursor_position;

                for (long cur = cursor_position - 1; cur < index; cur++) // Shift characters to the left
                {
                    buffer[cur] = buffer[cur + 1];
                }

                bool cond = (index != initial_pos);

                index--;
                buffer[index] = '\0';

                cursor_position--;

                if (cond)
                {
                    printf("\033[%ldC", (index - cursor_position));
                }

                putc('\b'); // Move cursor back
                putc(' ');  // Clear the character
                putc('\b'); // Move cursor back again

                if (cond)
                {
                    printf("\033[%ldD", index - cursor_position); // Analogous to the above putc sequence, but for multiple characters
                    printf("%s", buffer + cursor_position);
                    printf("\033[%ldD", index - cursor_position);
                }
            }
        }
        else
        {
            putc(character); // Echo the character back

            long initial_pos = cursor_position;

            for (long cur = index; cur >= cursor_position; cur--) // Shift characters to the right
            {
                buffer[cur + 1] = buffer[cur];
            }

            buffer[cursor_position] = character; // Store the character in the buffer

            if (index != initial_pos)
            {
                puts(buffer + cursor_position + 1);
                printf("\033[%ldD", index - initial_pos);
            }

            cursor_position++;
            index++;
        }
    }
    buffer[index] = '\0'; // Null-terminate the string
}
