/*
 * tolower.c
 *
 * Часть стандарта C2011
 *
 * Функция преобразует символ в нижний регистр
 *
 */

#include <ctype.h>

int tolower(int c)
{
  if (isupper(c))
    return c - 'A' + 'a';
  else
    return c;
}
