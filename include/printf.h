#ifndef PRINTF_H
#define PRINTF_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void puts(const char *s);
void getlines(char *restrict buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif // PRINTF_H
