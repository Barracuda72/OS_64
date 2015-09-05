/*
 * ispunct.c
 *
 * Часть стандарта C2011
 *
 * Функция проверяет, является ли символ пунктуационным
 *
 */

#include <ctype.h>

int ispunct(int c)
{
  return (isprint(c) && !isspace(c) && !isalnum(c));
}
