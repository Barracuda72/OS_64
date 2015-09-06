/*
 * time.c
 *
 * Часть стандарта C2011
 *
 * Функция определяет текущее календарное время
 *
 */

#include <time.h>

// TODO: реализовать!

time_t time(time_t *timer)
{
  time_t res = -1;

  if (timer != NULL)
    *timer = res;

  return res;
}
