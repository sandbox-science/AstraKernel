#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void puts(const char *s);
    void printf(char *s, ...);
    void getlines(char *restrict buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif // PRINTF_H
