#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  int strcmp(const char *str_1, const char *str_2);
  size_t strlen(const char *str);

#ifdef __cplusplus
}
#endif

#endif // STRING_H
