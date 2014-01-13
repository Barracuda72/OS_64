#ifndef __VFS_H__
#define __VFS_H__

#include <stdint.h>
#include <dirent.h>

/*
 * VFS - самая важная часть любой UNIX-like системы
 *
 * Здесь содержатся все базовые определения типов для остальной ФС
 */

/*
 * Forward declaration
 */

struct vfs_node;

/*
 * Первые 4 определены POSIX, последние 2 - стандарт де-факто
 */

typedef uint64_t (*read_type_t) (struct vfs_node *, uint64_t, uint64_t, uint8_t *);
typedef uint64_t (*write_type_t)(struct vfs_node *, uint64_t, uint64_t, uint8_t *);
typedef uint64_t (*open_type_t) (struct vfs_node *, uint32_t);
typedef uint64_t (*close_type_t)(struct vfs_node *);

typedef struct   dirent * (*readdir_type_t)(struct vfs_node *, uint64_t);
typedef struct   vfs_node * (*finddir_type_t)(struct vfs_node *, char *); 

typedef uint64_t (*init_type_t) (struct vfs_node *);
typedef uint64_t (*fini_type_t) (struct vfs_node *);
/*
 * Структура, описывающая функции драйвера файловой системы
 */

typedef struct vfs_driver
{
  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir;
  finddir_type_t finddir;
  init_type_t init;
  fini_type_t fini;
} vfs_driver_t;

/*
 * Ключевая структура - узел ФС
 */

typedef struct vfs_node
{
  char name [MAXNAMLEN+1]; // Имя записи
  uint32_t rights;         // Права доступа
  uint32_t flags;          // Флаги, включая тип
  uint64_t uid;            // Владелец
  uint64_t gid;            // Группа
  uint64_t inode;          // Inode
  uint64_t length;         // Размер файла
  uint64_t reserved;       // Зарезервировано
  
  vfs_driver_t *drv;       // Указывает, к какой ФС относится файл

  struct vfs_node *ptr;    // Используется символьными ссылками и точками монтирования
} vfs_node_t;

/*
 * Корень файловой системы
 */

extern vfs_node_t *vfs_root;

/*
 * Общие функции VFS
 */
uint64_t vfs_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t vfs_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t vfs_open(vfs_node_t *node, uint32_t flags);
uint64_t vfs_close(vfs_node_t *node);
struct dirent *vfs_readdir(vfs_node_t *node, uint64_t index);
vfs_node_t *vfs_finddir(vfs_node_t *node, char *name);
uint64_t vfs_mount(vfs_node_t *what, vfs_node_t *where);
uint64_t vfs_umount(vfs_node_t *node);

/*
 * Вспомогательные функции
 */
vfs_node_t *vfs_alloc_node();
struct dirent *vfs_alloc_dirent();

#endif // __VFS_H__

