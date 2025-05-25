#include "printf.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

// TODO: Check working of printf, all cases

_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");

// Memory-mapped I/O registers for UART0 on QEMU versatilepb
#define UART0_DR (*(volatile uint32_t *)0x101f1000) // Data Register
#define UART0_FR (*(volatile uint32_t *)0x101f1018) // Flag Register

static const uint32_t UART_FR_TXFF = (1U << 5); // Transmit FIFO Full
static const uint32_t UART_FR_RXFE = (1U << 4); // Receive FIFO Empty

// Send a single character over UART, waiting if the FIFO is full
static inline void putc(char c)
{
    // Wait until UART transmit FIFO is not full
    while (UART0_FR & UART_FR_TXFF)
        ;
    UART0_DR = (uint32_t)c;
}

unsigned long long _bdiv(unsigned long long dividend, unsigned long long divisor, unsigned long long *remainder)
{
    // INFO: Currently, this algorithm involves dividing only by 10 and 16. So, division by zero should not be a problem, yet.
    // TO-DO: Design a faster division algorithm and ensure that division by zero is not allowed.
    *remainder = 0;
    unsigned long long quotient = 0;

    for (int i = 63; i >= 0; i--)
    {
        quotient <<= 1;
        *remainder <<= 1;
        unsigned long long temp = (unsigned long long)1 << i; // Without this cast, the type is misinterpreted leading to UB
        *remainder |= (dividend & temp) >> i;

        if (*remainder >= divisor)
        {
            *remainder -= divisor;
            quotient |= 1;
        }
    }

    return quotient;
}

void _putunsignedlong(unsigned long long unum, unsigned long long base, bool hex_capital)
{
    char out_buf[32];
    uint32_t len = 0;

    char base16_factor = (7 * (hex_capital) + 39 * (!hex_capital)) * (base == 16); // If base 16, add 7 or 39 depending on
                                                                                   // X or x respectively

    unsigned long long mod;
    unsigned long long res;

    do
    {
        res = _bdiv(unum, base, &mod);
        out_buf[len] = '0' + mod + base16_factor * (mod > 9);

        len++;
        unum = res;
    } while (unum);

    for (uint32_t i = len; i > 0; i--)
    {
        putc(out_buf[i - 1]);
    }
}

void _putunsignedint(uint32_t unum)
{
    _putunsignedlong(unum, 10, false);
}

void _puthexsmall(unsigned long long unum)
{
    _putunsignedlong(unum, 16, false);
}

void _puthexcapital(unsigned long long unum)
{
    _putunsignedlong(unum, 16, true);
}

void _putintegers(char control, Format_State *format_state)
{
    format_state->in_format = false; // Valid as only %l updates this

    switch (control)
    {
    case 'u':
    {
        _putunsignedlong(format_state->num, 10, false);
        break;
    }

    case 'd':
    {
        uint32_t shamt = format_state->long_format * 63 + !(format_state->long_format) * 31;

        uint32_t sign_bit = (format_state->num) >> shamt;

        if (sign_bit)
        {
            putc('-');

            format_state->num = ((format_state->num ^ 0xffffffff) * !format_state->long_format +         // 2's complement for 32 bit
                                 (format_state->num ^ 0xffffffffffffffff) * format_state->long_format) + // 2's complement for 64 bit
                                1;
        }

        _putunsignedlong(format_state->num, 10, false);

        break;
    }
    case 'x':
    {
        _puthexsmall(format_state->num);
        break;
    }
    case 'X':
    {
        _puthexcapital(format_state->num);
        break;
    }
    }
}

// Validation for integer formats
bool _validate_format_specifier(char c)
{
    return (c == 'd') ||
           (c == 'u') ||
           (c == 'x') ||
           (c == 'X');
}

// Send a null-terminated string over UART
void puts(const char *s)
{
    while (*s)
    {
        putc(*s++);
    }
}

// Send a formatted string over UART
void printf(char *s, ...)
{
    va_list elem_list;

    va_start(elem_list, s);

    Format_State format_state = {.num = 0, .valid_format = false, .in_format = false, .long_format = false};

    while (*s)
    {
        if (*s == '%' || format_state.in_format)
        {
            switch (*(s + 1))
            {
            case 'c':
            {
                uint32_t character = va_arg(elem_list, uint32_t); // Characters are converted into 'int' when passed as var-args
                putc((char)character);
                break;
            }
            case 's':
            {
                char *it = va_arg(elem_list, char *);

                if (it == NULL)
                {
                    printf("(null)");
                    break;
                }

                while (*it)
                {
                    putc(*it++);
                }
                break;
            }
            case 'l':
            {
                format_state.in_format = true;
                format_state.long_format = true;
                s += 1; // Evaluate the immediate next char
                continue;
            }
            case '%':
            {
                putc('%');
                break;
            }
            default:
            {
                format_state.valid_format = _validate_format_specifier(*(s + 1));

                if (format_state.valid_format)
                {
                    if (format_state.long_format)
                    {
                        format_state.num = (unsigned long long)va_arg(elem_list, unsigned long long);
                    }
                    else
                    {
                        format_state.num = (uint32_t)va_arg(elem_list, uint32_t);
                    }

                    _putintegers(*(s + 1), &format_state);

                    format_state.long_format = false;
                }
                else
                {
                    // Invalid format error handling goes here
                }

                break;
            }
            }

            s += 2; // Skip format specifier
        }
        else
        {
            putc(*s++);
        }
    }

    va_end(elem_list);
}

// Function to get user input from UART
static inline char getc(void)
{
    // Wait until UART receive FIFO is not empty
    while (UART0_FR & UART_FR_RXFE)
        ;
    return (char)(UART0_DR & 0xFF);
}

// Function to getline from user input
void getlines(char *restrict buffer, size_t length)
{
    long long index = 0;
    long long cursor_position = 0;

    char character;

    uint8_t escape = 0;
    uint8_t arrow_keys = 0;

    while (index < length - 1)
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
        if ((character == '\b' || character == 0x7F) && index > 0) // Check for backspace
        {
            if (cursor_position > 0)
            {
                long long initial_pos = cursor_position;

                for (long long cur = cursor_position - 1; cur < index; cur++) // Shift characters to the left
                {
                    buffer[cur] = buffer[cur + 1];
                }

                bool cond = (index != initial_pos);

                index--;
                buffer[index] = '\0';

                cursor_position--;

                if (cond)
                    printf("\033[%ldC", (index - cursor_position));

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

            long long initial_pos = cursor_position;

            for (long long cur = index; cur >= cursor_position; cur--) // Shift characters to the right
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
