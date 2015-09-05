/*
 * isalnum.c
 *
 * Часть стандарта C2011
 *
 * Функция проверяет, является ли символ буквой или цифрой
 *
 */

#include <ctype.h>

int isalnum(int c)
{
  return (isalpha(c)||isdigit(c));
}
