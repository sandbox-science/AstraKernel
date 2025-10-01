#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct Format_State
    {
        unsigned long long num;
        bool valid_format;
        bool in_format;   // Used to handle multi-character format specifiers
        bool long_format; // %l. type specifier
    } Format_State;

    void puts(const char *s);
    void printf(const char *s, ...);
    void getlines(char *restrict buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif // PRINTF_H
