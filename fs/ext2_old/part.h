#ifndef __PART_H__
#define __PART_H__

#include "type.h"

/*
 * Описание таблицы разделов формата DOS
 */


/*
 * Описание раздела
 */
typedef struct __attribute__((packed))
{
  uint8_t  active;
  uint8_t  s_head;
  uint16_t s_sector : 6;
  uint16_t s_track : 10;
  uint8_t  type;
  uint8_t  e_head;
  uint16_t e_sector : 6;
  uint16_t e_track : 10;
  uint32_t first_sector;
  uint32_t num_sectors;
} partition;

/*
 * Сама таблица
 */
typedef struct
{
  char bootldr[446];
  partition parts[4];
} part;

#endif //__PART_H__
