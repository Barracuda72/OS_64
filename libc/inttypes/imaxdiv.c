/*
 * imaxdiv.c
 *
 * Часть стандарта C2011
 *
 * Функция выполняет целочисленное деление
 *
 */

#include <stdint.h>
#include <inttypes.h>

imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom)
{
  imaxdiv_t res;
  res.quot = numer/denom;
  res.rem = numer%denom;
  return res;
}
