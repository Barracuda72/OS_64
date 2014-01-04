#ifndef __MDPART_H__
#define __MDPART_H__

/*
 * Схема разделов в стиле MS-DOS
 */

#include <vfs.h>

int mdpart_init(vfs_node_t *node);

uint64_t mdpart_open(vfs_node_t *node, uint32_t flags);
uint64_t mdpart_close(vfs_node_t *node);
uint64_t mdpart_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t mdpart_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);

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
  uint16_t magic;
} part;

#define MDPART_MAGIC 0xAA55
#define MDPART_BLKSZ 512

#endif //__MDPART_H__
