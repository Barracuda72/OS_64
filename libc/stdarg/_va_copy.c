/*
 * _va_copy.c
 *
 * Часть стандарта C2011
 *
 * Функция копирует список аргументов переменной длины
 *
 */

#include <stdarg.h>

void _va_copy(va_list *dest, va_list *src)
{
  *dest = *src;
}
