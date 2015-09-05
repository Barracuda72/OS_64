/*
 * wcstoumax.c
 *
 * Часть стандарта C2011
 *
 * Функция преобразует "широкую" строку в целое число без знака
 *
 */

#include <stddef.h> // Для wchar_t
#include <stdint.h>
#include <inttypes.h>

#include <errno.h> // Для errno

#include <ctype.h> // Для isalnum

#include <locale.h> // Для NULL

uintmax_t wcstoumax(const wchar_t * restrict nptr,
                   wchar_t ** restrict endptr, int base)
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
    *endptr = nptr;
  return res;
}
