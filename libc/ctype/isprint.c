/*
 * isprint.c
 *
 * Часть стандарта C2011
 *
 * Функция проверяет, является ли символ печатаемым
 *
 */

#include <ctype.h>

int isprint(int c)
{
  return ((c >= 0x20) && (c <= 0x7E));
}
