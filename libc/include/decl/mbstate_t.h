/*
 * decl/mbstate_t.h
 *
 * Отдельный заголовочный файл
 *
 * Объявление типа mbstate_t
 *
 */

#ifndef _DECL_MBSTATE_T_H
#define _DECL_MBSTATE_T_H 1

#include <stdint.h>

typedef struct
{
  // Символ или его часть
  uint32_t ch;
  // Статус
  uint32_t st;
  // Позиция в строке
  uint32_t pos;
} mbstate_t;

#endif // _DECL_MBSTATE_T_H
