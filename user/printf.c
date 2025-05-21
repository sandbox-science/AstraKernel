#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

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

    for (int i = len - 1; i > -1; i--)
    {
        putc(out_buf[i]);
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

// Send a null-terminated string over UART
void puts(char *s, ...)
{
    va_list elem_list;

    va_start(elem_list, s);

    while (*s)
    {
        if (*s == '%')
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

                while (*it)
                {
                    putc(*it++);
                }
                break;
            }
            case 'l':
            {
                unsigned long long unum = va_arg(elem_list, unsigned long long);

                switch (*(s + 2))
                {
                case 'u':
                {
                    _putunsignedlong(unum, 10, false);
                    s += 1;
                    break;
                }

                case 'd':
                {

                    uint32_t sign_bit = unum >> 63;

                    if (sign_bit)
                    {
                        putc('-');
                        unum = ~unum + 1; // 2's complement
                    }

                    s += 1;
                    _putunsignedlong(unum, 10, false);
                    break;
                }

                case 'x':
                {
                    _puthexsmall(unum);
                    s += 1;
                    break;
                }
                case 'X':
                {
                    _puthexcapital(unum);
                    s += 1;
                    break;
                }
                default:
                {
                    s += 1;
                    break;
                }
                }

                break;
            }
            case 'd':
            {
                uint32_t num = va_arg(elem_list, uint32_t);

                uint32_t sign_bit = num >> 31;

                if (sign_bit)
                {
                    putc('-');
                    num = ~num + 1; // 2's complement
                }

                _putunsignedint(num);
                break;
            }
            case 'x':
            {
                uint32_t num = va_arg(elem_list, uint32_t);
                _puthexsmall(num);
                break;
            }
            case 'X':
            {
                uint32_t num = va_arg(elem_list, uint32_t);
                _puthexcapital(num);
                break;
            }
            case '%':
            {
                putc('%');
                break;
            }
            default:
                break;
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
    size_t index = 0;
    char character;
    while (index < length - 1)
    {
        character = getc();
        if (character == '\r') // Check for carriage return
        {
            break;
        }
        if ((character == '\b' || character == 0x7F) && index > 0) // Check for backspace
        {
            index--;
            putc('\b'); // Move cursor back
            putc(' ');  // Clear the character
            putc('\b'); // Move cursor back again
        }
        else
        {
            buffer[index++] = character; // Store the character in the buffer
            putc(character);             // Echo the character back
        }
    }
    buffer[index] = '\0'; // Null-terminate the string
}