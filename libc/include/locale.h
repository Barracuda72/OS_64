/*
 * locale.h
 *
 * Часть стандарта C2011
 *
 * Локализация приложений
 *
 */

#ifndef _LOCALE_H
#define _LOCALE_H 1

/*
 * Структура, содержащая информацию о форматировании дат, сумм и чисел в
 * рамках локали
 */
struct lconv
{
  char *decimal_point;
  char *thousands_sep;
  char *grouping;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char *currency_symbol;
  char frac_digits;
  char p_cs_precedes;
  char n_cs_precedes;
  char p_sep_by_space;
  char n_sep_by_space;
  char p_sign_posn;
  char n_sign_posn;
  char *int_curr_symbol;
  char int_frac_digits;
  char int_p_cs_precedes;
  char int_n_cs_precedes;
  char int_p_sep_by_space;
  char int_n_sep_by_space;
  char int_p_sign_posn;
  char int_n_sign_posn;
};

/*
 * Макросы
 */
#define NULL ((void *)0)

/*
 * Параметры для setlocale
 */
#define LC_ALL      0
#define LC_COLLATE  1 // Влияет на strcoll и strxfrm
#define LC_CTYPE    2 // Влияет на функции работы с символами 
#define LC_MONETARY 3 // Влияет на значение, возвращаемое localeconv
#define LC_NUMERIC  4 // Влияет на форматирование чисел
#define LC_TIME     5 // Влияет на strftime и wcsftime

/*
 * Функции
 */

/*
 * Изменяет текущую локаль или ее часть
 */
char *setlocale(int category, const char *locale);

/*
 * Возвращает структуру lconv текущей локали
 */
struct lconv *localeconv(void);

#endif // _LOCALE_H
