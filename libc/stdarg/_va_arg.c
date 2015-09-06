/*
 * _va_arg.c
 *
 * Часть стандарта C2011
 *
 * Функция возвращает следующий аргумент из списка аргументов переменной длины
 *
 */

#include <stdarg.h>

void *_va_arg(va_list *ap, int size)
{
  void *ptr = (void *)*((long *)*ap);
  (*ap) += 8;//size;
  return ptr;
}
