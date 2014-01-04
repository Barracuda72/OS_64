#ifndef __DIRENT_H__
#define __DIRENT_H__

/*
 * Заголовочный файл, требуемый по стандарту POSIX
 * 
 * Определяет структуру dirent и сопутствующие ей сущности
 */

/*
 * Максимальная длина имени (константа POSIX)
 */

#define MAXNAMLEN 255


/*
 * Структура, возвращаемая вызовом readdir
 */
struct dirent 
{
  char name [MAXNAMLEN+1]; // Имя элемента
  uint64_t inode;     // Номер Inode
}; 

/*
 * Тип записи
 */

#define VFS_UNKNOWN     0x00
#define VFS_FILE        0x01
#define VFS_DIRECTORY   0x02
#define VFS_CHARDEV     0x03
#define VFS_BLOCKDEV    0x04
#define VFS_PIPE        0x05
#define VFS_SYMLINK     0x06
#define VFS_MOUNTPOINT  0x08

#define VFS_STORAGE     0x100

#endif //__DIRENT_H__
