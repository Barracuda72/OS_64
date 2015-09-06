/*
 * time.h
 *
 * Часть стандарта C2011
 *
 * Дата и время
 *
 */

#ifndef _TIME_H
#define _TIME_H 1

#include <decl/NULL.h>
#define CLOCKS_PER_SEC 1000000 // Задано POSIX
#define TIME_UTC 1

#include <decl/size_t.h> // Для size_t

/*
 * Типы для часов и времени
 */
// TODO: должны быть вещественными!
typedef unsigned long clock_t;
typedef unsigned long time_t;

struct timespec {
  time_t tv_sec;  // >= 0
  long tv_nsec;   // [0, 999999999]
};

struct tm {
  int tm_sec;   // Секунды      [0, 60]
  int tm_min;   // Минуты       [0, 59]
  int tm_hour;  // Часы         [0, 23]
  int tm_mday;  // День месяца  [1, 31]
  int tm_mon;   // Месяц в году [0, 11]
  int tm_year;  // Год, начиная от 1900 года  
  int tm_wday;  // День недели  [0, 6], 0 - воскресенье
  int tm_yday;  // День года    [0, 365]
  int tm_isdst; // Флаг дневного времени
};

/*
 * Функции
 */

/*
 * Возвращает использованное процессорное время
 */
clock_t clock(void);

/*
 * Вычисляет разность между двумя календарными временами
 */
// TODO: реализовать!
// double difftime(time_t time1, time_t time0);
time_t difftime(time_t time1, time_t time0);

/*
 * Преобразует время из разбитого на части представления в time_t
 */
time_t mktime(struct tm *timeptr);

/*
 * Определяет текущее календарное время
 */
time_t time(time_t *timer);

/*
 * Устанавливает значения структуры ts в соответствии с текущим календарным
 * временем
 */
int timespec_get(struct timespec *ts, int base);

/*
 * Преобразует значения структуры tm для составления текстового описания даты
 * и времени
 */
char *asctime(const struct tm *timeptr);

/*
 * Преобразует значения сруктуры tm для составления текстового описания
 * локальных даты и времени
 */
char *ctime(const time_t *timer);

/*
 * Преобразует календарное время, на которое указывает timer, в значения
 * структуры tm
 */
struct tm *gmtime(const time_t *timer);

/*
 * Преобразует локальное календарное время, на которое указывает timer, 
 * в значения структуры tm
 */
struct tm *localtime(const time_t *timer);

/*
 * Преобразует дату в соответствии с заданным форматом
 */
size_t strftime(char * restrict s, size_t maxsize,
                const char * restrict format,
                const struct tm * restrict timeptr
               );

#endif // _TIME_H
