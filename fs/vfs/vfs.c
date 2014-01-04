/*
 * Реализация VFS
 */

#include <stdint.h>
#include <vfs.h>
#include <errno.h>

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
      ((node->flags&VFS_DIRECTORY) == VFS_DIRECTORY) &&
      (node->drv != NULL) && 
      (node->drv->readdir != NULL))
    return node->drv->readdir(node, index);
  else
    return (struct dirent *)EBADF;
}

vfs_node_t *vfs_finddir (vfs_node_t *node, char *name)
{
  if ((node != NULL) &&
      ((node->flags&VFS_DIRECTORY) == VFS_DIRECTORY) &&
      (node->drv != NULL) && 
      (node->drv->finddir != NULL))
    return node->drv->finddir(node, name);
  else
    return (vfs_node_t *)EBADF;
}

