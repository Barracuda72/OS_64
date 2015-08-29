#include <kprintf.h>
#include <mem.h>
#include <dirent.h>
#include <errno.h>

#include <vfs.h>
#include <tty.h>

vfs_driver_t tty_drv = {
  tty_read,
  tty_write,
  tty_open,
  tty_close,
  NULL,
  NULL,
  tty_init,
  tty_fini
};

vfs_node_t *tty_init(vfs_node_t *node)
{
  vfs_node_t *res = NULL;

  res = vfs_alloc_node();
  strncpy(res->name, "tty", 3);
  res->name[3] = 0;
  res->rights = 660;
  res->gid = res->uid = 0;

  res->drv = &tty_drv;
  res->flags = VFS_CHARDEV|VFS_TTY;
  res->inode = 0;
  res->reserved = (uint64_t)NULL;
  res->ptr = NULL;

  return res;
}

uint64_t tty_fini(vfs_node_t *node)
{
  if (node == NULL)
    return EINVAL;

  kfree(node);
  return 0;
}

uint64_t tty_open(vfs_node_t *node, uint32_t flags)
{
  return 0;
}

// TODO
uint64_t tty_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if (node == NULL)
    return EINVAL;

  return EINVAL;
}

// TODO: ввести буферизацию!
uint64_t tty_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if (node == NULL)
    return EINVAL;

  if (offset != 0)
    return EINVAL;

  if (size == 0)
    return 0;

  int i;

  for (i = 0; i < size; i++)
    ktty_putc(buffer[i]);

  return size;
}

uint64_t tty_close(vfs_node_t *node)
{
  return 0;
}

