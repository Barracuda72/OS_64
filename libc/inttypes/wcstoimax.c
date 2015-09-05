/*
 * wcstoimax.c
 *
 * Часть стандарта C2011
 *
 * Функция преобразует "широкую" строку в целое число со знаком
 *
 */

#include <stddef.h> // Для wchar_t
#include <stdint.h>
#include <inttypes.h>

#include <errno.h>

intmax_t wcstoimax(const wchar_t * restrict nptr,
                   wchar_t ** restrict endptr, int base)
{
  intmax_t res;
  int sign;

  if (nptr[0] == '-')
  {
    nptr++;
    sign = -1;
  } else {
    sign = 1;
    if (nptr[0] == '+')
      nptr++;
  }

  res = wcstoumax(nptr, endptr, base);

  if (res == UINTMAX_MAX)
  {
    errno = ERANGE;

    if (sign == -1)
      return INTMAX_MIN;
    else
      return INTMAX_MAX;
  } else {
    return sign*res;
  }
}
