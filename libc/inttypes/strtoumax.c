/*
 * strtoumax.c
 *
 * Часть стандарта C2011
 *
 * Функция преобразует строку в целое число без знака
 *
 */

#include <stdint.h>
#include <inttypes.h>

#include <errno.h> // Для errno

#include <ctype.h> // Для isalnum

#include <locale.h> // Для NULL

uintmax_t strtoumax(const char * restrict nptr,
                   char ** restrict endptr, int base)
{
  uintmax_t res;

  res = 0;
  while (isalnum(*nptr) && ((tolower(*nptr) - '0') < base))
  {
    // Проверка переполнения
    if (res > res*base)
    {
      errno = ERANGE;
      res = UINTMAX_MAX;
      break;
    }

    res = (*nptr - '0') + res*base;
    nptr++;
  }

  if (endptr != NULL)
    *endptr = (char *)nptr;
  return res;
}
