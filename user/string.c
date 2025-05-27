#include "string.h"

/**
 * @brief Compares two null-terminated strings lexicographically.
 *
 * @param str_1 Pointer to the first string to compare.
 * @param str_2 Pointer to the second string to compare.
 * 
 * @return int 0 if strings are equal, -1 if `str_1` < `str_2`, 1 if `str_1` > `str_2`.
 */
int strcmp(const char *str_1, const char *str_2)
{
  unsigned char ch1, ch2;
  do
  {
    ch1 = *str_1++;
    ch2 = *str_2++;

    if (ch1 != ch2)
    {
      // NOTE: If needed in the future, can return the difference between
      // the strings rather than just -1 or 1
      return ch1 < ch2 ? -1 : 1;
    }

  } while (ch1);

  return 0;
}

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * @param str Pointer to the null-terminated string to be measured.
 * @return The number of characters in the string, excluding the null terminator.
 */
size_t strlen(const char *str)
{
  const char *s = str;
  while (*s)
  {
    ++s;
  }
  return s - str;
}
