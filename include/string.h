#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief Compares two null-terminated strings lexicographically.
   *
   * @param str_1 Pointer to the first string to compare.
   * @param str_2 Pointer to the second string to compare.
   * 
   * @return int 0 if strings are equal, -1 if `str_1` < `str_2`, 1 if `str_1` > `str_2`.
  */
  int strcmp(const char *str_1, const char *str_2);

  /**
   * @brief Calculates the length of a null-terminated string.
   *
   * @param str Pointer to the null-terminated string to be measured.
   * @return The number of characters in the string, excluding the null terminator.
  */
  size_t strlen(const char *str);

#ifdef __cplusplus
}
#endif

