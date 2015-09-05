/*
 * isgraph.c
 *
 * Часть стандарта C2011
 *
 * Функция проверяет, является ли символ отображаемым
 *
 */

#include <ctype.h>

int isgraph(int c)
{
  return ((c > 0x20) && (c <= 0x7E));
}
