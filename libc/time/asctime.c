/*
 * asctime.c
 *
 * Часть стандарта C2011
 *
 * Функция преобразует значения структуры tm для составления текстового
 * описания даты и времени
 *
 */

#include <stdio.h> // Для snprintf
#include <time.h>

static const char wday_name[7][3] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char mon_name[12][3] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char result[26];

char *asctime(const struct tm *timeptr)
{
  snprintf(result, 26, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
          wday_name[timeptr->tm_wday],
          mon_name[timeptr->tm_mon],
          timeptr->tm_mday, timeptr->tm_hour,
          timeptr->tm_min, timeptr->tm_sec,
          1900 + timeptr->tm_year
         );
  return result;
}
