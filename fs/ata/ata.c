#include <vfs.h>
#include <ata.h>
#include <mem.h>
#include <kprintf.h>
#include <errno.h>

/*
 * Драйвер ATA VFS
 */

vfs_driver_t ata_drv = {
  ata_read,
  ata_write,
  ata_open,
  ata_close,
  NULL,
  NULL,
  NULL,
  NULL
};

vfs_node_t *ata_init ()
{
  vfs_node_t *v = vfs_alloc_node();

  strncpy(v->name, "ata", 3);
  v->name[6] = 0;
  
  v->rights = 660;
  v->gid = v->uid = 0;
  v->flags = VFS_CHARDEV|VFS_STORAGE;

  v->inode = 0;

  v->reserved = -1; // TODO: HACK!
  v->length = 0xFFFFFFFF; // TODO: проверять размер диска!

  v->drv = &ata_drv;

  v->ptr = NULL;
  return v;
}

uint64_t ata_read (vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if ((node != NULL) &&
      (node->reserved != 0))
  {
    if ((offset > node->length) ||
        (buffer == NULL))
      return EINVAL;

    int len = offset+size > node->length ? node->length - offset : size;
    // printf("ATA PIO read %d bytes, offset %X\n", len, offset);

    int sectors = len>>9;
    if (len & 0x1FF)
      sectors++;

    char *read_buf = kmalloc(sectors<<9);

    ata_pio_read(read_buf, offset>>9, sectors);

    memcpy(buffer, read_buf+(offset&0x1FF), len);

    kfree(read_buf);

    return len;
  } else
    return EBADF;
}

uint64_t ata_write (vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
#if 0
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
#endif
  return EINVAL;
}

uint64_t ata_open (vfs_node_t *node, uint32_t flags)
{
  if (node != NULL)
    return 0;
  else
    return EBADF;
}

uint64_t ata_close (vfs_node_t *node)
{
  return 0;
}
