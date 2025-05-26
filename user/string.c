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
    unsigned char str1, str2;
    while(1)
    {
      str1 = *str_1++;
      str2 = *str_2++;
      
      if (str1 != str2)
      {
        // NOTE: If needed in the future, can return the difference between
        // the strings rather than just -1 or 1
        return str1 < str2 ? -1 : 1;
      }

      if (!str1)
      {
        break;
      }
    }

    return 0;
}