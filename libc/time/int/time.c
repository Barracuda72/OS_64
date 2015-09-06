/*
 * int/time.c
 *
 * Внутренние процедуры
 *
 * Функции для работы с датой и временем
 *
 */

#include <time.h>
#include <int/time.h>

/*
 * Возвращает 1, если год високосный, иначе 0
 */
int int_time_isleap(int year)
{
  return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

/*
 * Возвращает количество дней в месяце
 */
int int_time_days_in_month(int mon, int year)
{
  switch (mon)
  {
    case  0: /* Январь */
    case  2: /* Март */
    case  4: /* Май */
    case  6: /* Июль */
    case  7: /* Август */
    case  9: /* Октябрь */
    case 11: /* Декабрь */
      return 31;

    case  1: /* Февраль */
      return 28 + int_time_isleap(year);

    case  3: /* Апрель */
    case  5: /* Июнь */
    case  8: /* Сентябрь */
    case 10: /* Ноябрь */
    default: /* Пес его знает */
      return 30;
  }
}

/*
 * Возвращает количество дней с начала года до текущего месяца
 */
int int_time_days_upto_month(int mon, int year)
{
  int i, res;

  res = 0;

  for (i = 0; i < mon; i++)
    res += int_time_days_in_month(i, year);

  return res;
}

/*
 * Возвращает количество дней с начала эпохи (1970) до текущего года
 */
int int_time_days_upto_year(int year)
{
  int i, res;

  res = 0;
  for (i = 1970; i < year; i++)
    res += 365 + int_time_isleap(i);

  return res;
}

