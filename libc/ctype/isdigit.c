/*
 * isdigit.c
 *
 * Часть стандарта C2011
 *
 * Функция проверяет, является ли символ цифрой
 *
 */

#include <ctype.h>

int isdigit(int c)
{
  return ((c >= '0') && (c <= '9'));
}
