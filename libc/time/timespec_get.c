/*
 * timespec_get.c
 *
 * Часть стандарта C2011
 *
 * Функция устанавливает значения структуры ts в соответствии с текущим
 * календарным временем
 *
 */

#include <time.h>

// TODO: реализовать!

int timespec_get(struct timespec *ts, int base)
{
  // Постоянно находимся в 1.01.1970, 00:00
 
  ts->tv_sec = 0;
  ts->tv_nsec = 0;

  return base;
}
