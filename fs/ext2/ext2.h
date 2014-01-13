#ifndef __EXT2_H__
#define __EXT2_H__

#include <stdint.h>
#include <vfs.h>

/*
 * Константы драйвера
 */

#define EXT2_MAGIC 0xEF53
#define EXT2_ROOT_INODE 2
#define EXT2_MAX_PATH 256

#pragma pack(1)
/*
 * Суперблок
 */
typedef struct __attribute__((packed))
{
  /*
   * Общие поля
   */

  uint32_t inodes_num;  // Количество inode в файловой системе
  uint32_t blocks_num;  // Количество блоков в файловой системе
  uint32_t resrvd_blk;  // Количество зарезервированных блоков
  uint32_t free_blocks; // Количество свободных блоков
  uint32_t free_inodes; // Количество свободных inode
  uint32_t sblock_num;  // Номер блока, содержащего суперблок
  uint32_t block_size;  // log2(block_size) - 10, т.е 1024<<block_size
  uint32_t frag_size;   // То же самое для размера фрагмента
  uint32_t blk_in_grp;  // Количество блоков в группе
  uint32_t frg_in_grp;  // Количество фрагментов в группе
  uint32_t ind_in_grp;  // Количество inode в группе
  uint32_t mount_time;  // Время последнего монтирования
  uint32_t write_time;  // Время последней записи
  uint16_t mount_num;   // Количество монтирований с последней проверки
  uint16_t mount_max;   // Количество допустимых монтирований без проверки
  uint16_t magic;       // 0xEF53
  uint16_t state;       // Состояние файловой системы
  uint16_t error_hndl;  // Что делать при ошибках
  uint16_t ver_minor;   // Минорная версия
  uint32_t check_time;  // Время последней проверки
  uint32_t check_frcd;  // Интервал между принудительными проверками
  uint32_t os_id;       // Идентификатор ОС, создавшей систему
  uint32_t ver_major;   // Мажорная версия ФС
  uint16_t root_uid;    // UID пользователя зарезервированных блоков
  uint16_t root_gid;    // GID --//--

  /*
   * Расширенные поля, имеют смысл, если версия >= 1.0
   */

  uint32_t free_inode;  // Первый свободный inode, 11 в версии < 1.0
  uint16_t inode_size;  // Размер inode в байтах, 128 для < 1.0
  uint16_t sblock_grp;  // Группа блоков, к которой принадлежит суперблок
  uint32_t feature_opt; // Необязательные расширения ФС
  uint32_t feature_req; // Обязательные расширения
  uint32_t feature_ro;  // Расширения, требуемые для поддержки записи
  uint8_t  fs_uuid[16]; // ID файловой системы
  uint8_t  fs_name[16]; // Имя тома
  uint8_t  fs_mpnt[64]; // Последняя точка монтирования
  uint32_t compression; // Используемый метод сжатия
  uint8_t  prea_file;   // Количество блоков под файл (preallocate)
  uint8_t  prea_dir;    // --//-- под директорию
  uint16_t unused;
  uint8_t  jour_id[16]; // ID журнала (для EXT3)
  uint32_t j_inode;     // inode журнала
  uint32_t j_device;    // Устройство журналирования
  uint32_t orph_inodes; // Заголовок списка "осиротевших" inode
  uint8_t  rsrvd[788];
} ext2_superblock;

typedef struct __attribute__((packed))
{
  uint32_t block_bmp;  // Адрес карты использования блоков
  uint32_t inode_bmp;  // Адрес карты использования inode
  uint32_t inode_st;   // Адрес начала таблицы inode
  uint16_t free_blk;   // Количество свободных блоков в группе
  uint16_t free_ino;   // Количество свободных inode
  uint16_t dir_num;    // Количество директорий в группе
  uint8_t  unused[14];
} ext2_group_desc;

typedef struct __attribute__((packed))
{
  uint16_t type_perm;  // Тип и разрешения
  uint16_t uid;        // UID хозяина
  uint32_t size_low;   // Младшие 32 бита размера файла
  uint32_t acc_time;   // Время последнего доступа
  uint32_t cre_time;   // Время создания
  uint32_t mod_time;   // Время последнего изменения
  uint32_t del_time;   // Время удаления
  uint16_t gid;        // GID хозяина
  uint16_t links;      // Количество жестких ссылок на этот файл
  uint32_t sectors;    // Количество секторов, которое занимает этот файл
  uint32_t flags;      // Флаги
  uint32_t oss_value1; // Отведено для ОС
  uint32_t direct[12]; // Direct Block Pointer 0-11
  uint32_t s_indirect; // Single Indirect Block Pointer
  uint32_t d_indirect; // Double Indirect Block Pointer
  uint32_t t_indirect; // Triple Indirect Block Pointer
  uint32_t generation; // Номер поколения (для NFS)
  uint32_t file_acl;   // ACL файла для версии >= 1.0
  union
  {
    uint32_t size_hi;  // Старшие 32 бита размера, или ACL директории
    uint32_t dir_acl;
  };
  uint32_t frag_addr;  // Адрес фрагмента
  /*
   * Значения, специфичные для конкретной ОС
   * В данном случае взяты значения, используемые LINUX
   */
  uint8_t  frag_num;   // Номер фрагмента
  uint8_t  frag_size;  // Размер фрагмента
  uint16_t rsrvd1;
  uint16_t uid_hi;     // Старшие 16 бит 32-битного UID
  uint16_t gid_hi;     // Старшие 16 бит 32-битного GID
  uint32_t rsrvd2;

} ext2_inode;

typedef struct __attribute__((packed))
{
  uint32_t inode;      // inode файла
  uint16_t size;       // Размер данной записи
  uint8_t  lenght;     // Длина имени
  union
  {
    uint8_t lenght_hi;
    uint8_t type;
  };
  char name[0];
} ext2_direntry;
#pragma pack()

/*
 * Состояния файловой системы
 */
enum
{
  EXT2_STATE_CLEAN = 1,
  EXT2_STATE_ERROR
};

/*
 * Методы обработки ошибок ФС
 */
enum
{
  EXT2_ERR_IGNORE = 1,
  EXT2_ERR_MOUNT_RO,
  EXT2_ERR_PANIC
};

/*
 * Операционные системы, создающие ФС
 */
enum
{
  EXT2_OS_LINUX = 0,
  EXT2_OS_HURD,
  EXT2_OS_MASIX,
  EXT2_OS_FREEBSD,
  EXT2_OS_BSDLITE
};

/*
 * Тип inode
 */
enum
{
  EXT2_INODE_FIFO = 0x1000,
  EXT2_INODE_CHAR = 0x2000,
  EXT2_INODE_DIR  = 0x4000,
  EXT2_INODE_BLK  = 0x6000,
  EXT2_INODE_FILE = 0x8000,
  EXT2_INODE_LINK = 0xA000,
  EXT2_INODE_SOCK = 0xC000
};

/*
 * Разрешения
 */
enum
{
  EXT2_PERM_O_EXEC = 0x001,
  EXT2_PERM_O_WRIT = 0x002,
  EXT2_PERM_O_READ = 0x004,

  EXT2_PERM_G_EXEC = 0x008,
  EXT2_PERM_G_WRIT = 0x010,
  EXT2_PERM_G_READ = 0x020,

  EXT2_PERM_U_EXEC = 0x040,
  EXT2_PERM_U_WRIT = 0x080,
  EXT2_PERM_U_READ = 0x100,

  EXT2_PERM_STICKY = 0x200,
  EXT2_PERM_SETGID = 0x400,
  EXT2_PERM_SETUID = 0x800
};

uint64_t ext2_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t ext2_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t ext2_open(vfs_node_t *node, uint32_t flags);
uint64_t ext2_close(vfs_node_t *node);
struct dirent *ext2_readdir(vfs_node_t *node, uint64_t index);
vfs_node_t *ext2_finddir(vfs_node_t *node, char *name);

int ext2_init(vfs_node_t *node);

#endif // __EXT2_H__
