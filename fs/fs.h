#ifndef __FS_H__
#define __FS_H__

#include <stdint.h>
#include <vfs.h>

// Корневой узел
extern vfs_node_t *root;

#ifdef __HOSTED__
void fs_init(void *p, int len);
#else
void fs_init(void);
#endif

/*
 * Структура, описывающая открытый файл
 */
typedef struct _fs_open_desc
{
  // Узел открытого файла
  vfs_node_t *node;
  // Смещение
  uint64_t offset;
  // Флаги
  uint64_t flags;
} fs_open_desc;

// Максимальное количество открытых процессом файлов
#define MAX_OPEN_FILES 16

#endif // __FS_H__
