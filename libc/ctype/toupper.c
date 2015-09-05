/*
 * toupper.c
 *
 * Часть стандарта C2011
 *
 * Функция преобразует символ в верхний регистр
 *
 */

#include <ctype.h>

int toupper(int c)
{
  if (islower(c))
    return c - 'a' + 'A';
  else
    return c;
}
