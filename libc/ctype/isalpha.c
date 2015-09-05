/*
 * isalpha.c
 *
 * Часть стандарта C2011
 *
 * Функция проверяет, является ли символ буквой
 *
 */

#include <ctype.h>

int isalpha(int c)
{
  return (isupper(c) || islower(c));
}
