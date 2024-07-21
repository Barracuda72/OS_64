/*
 * Реализация VFS
 */

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <vfs.h>
#include <errno.h>
#include <mem.h>

#include <ext2.h>
#include <fat32.h>

vfs_node_t *vfs_root = NULL;

uint64_t vfs_read (vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if ((node != NULL) &&
      (node->drv != NULL) && 
      (node->drv->read != NULL))
    return node->drv->read(node, offset, size, buffer);
  else
    return EBADF;
}

uint64_t vfs_write (vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if ((node != NULL) &&
      (node->drv != NULL) && 
      (node->drv->write != NULL))
    return node->drv->write(node, offset, size, buffer);
  else
    return EBADF;
}

uint64_t vfs_open (vfs_node_t *node, uint32_t flags)
{
  if ((node != NULL) &&
      (node->drv != NULL) && 
      (node->drv->open != NULL))
    return node->drv->open(node, flags);
  else
    return EBADF;
}

uint64_t vfs_close (vfs_node_t *node)
{
  if ((node != NULL) &&
      (node->drv != NULL) && 
      (node->drv->close != NULL))
    return node->drv->close(node);
  else
    return EBADF;
}

struct dirent *vfs_readdir (vfs_node_t *node, uint64_t index)
{
  if ((node != NULL) &&
      (node->flags&VFS_DIRECTORY))
  {
    if (node->flags&VFS_MOUNTPOINT)
      return vfs_readdir(node->ptr, index);

    else if ((node->drv != NULL) && 
             (node->drv->readdir != NULL))
      return node->drv->readdir(node, index);
  }

  return (struct dirent *)EBADF;
}

vfs_node_t *vfs_finddir (vfs_node_t *node, char *name)
{
  if ((node != NULL) &&
      (node->flags&VFS_DIRECTORY))
  {
    if (node->flags&VFS_MOUNTPOINT)
      return vfs_finddir(node->ptr, name);
      
    else if ((node->drv != NULL) && 
             (node->drv->finddir != NULL))
      return node->drv->finddir(node, name);
  }

  return (vfs_node_t *)EBADF;
}

vfs_node_t *vfs_init_fs(vfs_node_t *node)
{
  vfs_node_t *v = NULL;

  if((v = ext2_init(node)) != NULL)
    return v;

  if((v = fat32_init(node)) != NULL)
    return v;

  return NULL;
}

uint64_t vfs_fini_fs(vfs_node_t *node)
{
  if ((node == NULL)||
      (node->drv == NULL)||
      (node->drv->fini == NULL))
    return EINVAL;

  return node->drv->fini(node);
}

uint64_t vfs_mount (vfs_node_t *what, vfs_node_t *where)
{
  if ((what == NULL) || (where == NULL))
    return EINVAL;

  if (!(where->flags&(VFS_DIRECTORY)))
    return ENOTDIR;

  if ((where->flags&(VFS_MOUNTPOINT)) != 0)
    return EBUSY;

  /*
   * По идее, нужно проверять еще условие "каталог не пуст"
   * Впрочем, например, Linux позволяет определенным ключом
   * указать необходимость монтирования в непустой каталог.
   * Поэтому - пока забьем.
   */

  /*
   * Мы создаем новый узел
   * В нем ptr будет указывать на подключаемый узел,
   * а сам узел будет представлять собой корень подключаемой ФС
   */
  vfs_node_t *v = vfs_init_fs(what);
  if (!v)
    return EXDEV;

  where->flags |= VFS_MOUNTPOINT;
  where->ptr = v;
  return 0;
}

uint64_t vfs_umount (vfs_node_t *node)
{
  if (node == NULL)
    return EINVAL;

  if (!(node->flags&VFS_DIRECTORY))
    return ENOTDIR;

  if (!(node->flags&VFS_MOUNTPOINT))
    return ENOENT;

  uint64_t res = vfs_fini_fs(node->ptr);

  node->ptr = NULL;

  return res;
}

vfs_node_t *vfs_alloc_node()
{
  vfs_node_t *res = kmalloc(sizeof(vfs_node_t));
  if (res != NULL)
    memset(res, 0, sizeof(vfs_node_t));
  return res;
}

struct dirent *vfs_alloc_dirent()
{
  struct dirent *res = kmalloc(sizeof(struct dirent));
  if (res != NULL)
    memset(res, 0, sizeof(struct dirent));
  return res;
}

