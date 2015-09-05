/*
 * isblank.c
 *
 * Часть стандарта C2011
 *
 * Функция проверяет, является ли символ пробельным
 *
 */

#include <ctype.h>

int isblank(int c)
{
  return ((c == ' ') || (c == '\t'));
}
