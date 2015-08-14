#include <vfs.h>
#include <initrd.h>
#include <mem.h>
#include <kprintf.h>
#include <errno.h>

/*
 * INITal Ram Disk
 */

vfs_driver_t initrd_drv = {
  initrd_read,
  initrd_write,
  initrd_open,
  initrd_close,
  NULL,
  NULL,
  NULL,
  NULL
};

vfs_node_t *initrd_init (void *p, uint64_t size)
{
  vfs_node_t *v = vfs_alloc_node();

  strncpy(v->name, "initrd", 6);
  v->name[6] = 0;
  
  v->rights = 660;
  v->gid = v->uid = 0;
  v->flags = VFS_CHARDEV|VFS_STORAGE;

  v->inode = 0;

  v->reserved = (uint64_t)p;
  v->length = size;

  v->drv = &initrd_drv;

  v->ptr = NULL;
  return v;
}

uint64_t initrd_read (vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if ((node != NULL) &&
      (node->reserved != 0))
  {
    if ((offset > node->length) ||
        (buffer == NULL))
      return EINVAL;

    int len = offset+size > node->length ? node->length - offset : size;
    void *p = (void *)(node->reserved+offset);

    memcpy(buffer, p, len);

    return len;
  } else
    return EBADF;
}

uint64_t initrd_write (vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if ((node != NULL) &&
      (node->reserved != 0))
  {
    if ((offset > node->length) ||
        (buffer == NULL))
      return EINVAL;

    int len = offset+size > node->length ? node->length - offset : size;
    void *p = (void *)(node->reserved+offset);

    memcpy(p, buffer, len);

    return len;
  } else
    return EBADF;
}

uint64_t initrd_open (vfs_node_t *node, uint32_t flags)
{
  if (node != NULL)
    return 0;
  else
    return EBADF;
}

uint64_t initrd_close (vfs_node_t *node)
{
  return 0;
}
