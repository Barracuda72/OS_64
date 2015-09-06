/*
 * int/time.h
 *
 * Внутренние процедуры
 *
 * Функции для работы с датой и временем
 *
 */

#ifndef _INT_TIME_H
#define _INT_TIME_H 1

/*
 * Возвращает 1, если год високосный, иначе 0
 */
int int_time_isleap(int year);

/*
 * Возвращает количество дней в месяце
 */
int int_time_days_in_month(int mon, int year);

/*
 * Возвращает количество дней с начала года до текущего месяца
 */
int int_time_days_upto_month(int mon, int year);

/*
 * Возвращает количество дней с начала эпохи (1970) до текущего года
 */
int int_time_days_upto_year(int year);

#endif // _INT_TIME_H
