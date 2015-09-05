/*
 * imaxabs.c
 *
 * Часть стандарта C2011
 *
 * Функция вычисляет абсолютное значение параметра
 *
 */

#include <stdint.h>
#include <inttypes.h>

intmax_t imaxabs(intmax_t j)
{
  if (j >= 0)
    return j;
  else
    return -j;
}
