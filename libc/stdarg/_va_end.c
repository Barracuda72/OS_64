/*
 * _va_end.c
 *
 * Часть стандарта C2011
 *
 * Функция уничтожает список аргументов переменной длины
 *
 */

#include <stdarg.h>
#include <decl/NULL.h> // Для NULL

void _va_end(va_list *ap)
{
  *ap = NULL;
}
