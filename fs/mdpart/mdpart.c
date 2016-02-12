/*
 * Схема разделов в стиле MS-DOS
 */

#include <mdpart.h>
#include <devfs.h>

#include <stddef.h>

#include <errno.h>

vfs_driver_t mdpart_drv = {
  mdpart_read,
  mdpart_write,
  mdpart_open,
  mdpart_close,
  NULL,
  NULL,
  NULL,
  NULL
};

int mdpart_init(vfs_node_t *node)
{
  part buf;
  int i;

  int l = vfs_read(node, 0, 512, (uint8_t *)&buf);

  if ((l < 512) || (buf.magic != MDPART_MAGIC))
    return -1;

  /*
   * Тут должна быть какая-то хитрая эвристика для определения
   * корректности таблицы разделов.
   * Но ее нет
   */
  for (i = 0; i < 4; i++)
    if (buf.parts[i].active&(~MDPART_ACTIVE) != 0)
      return -2;

  /*
   * Найдена таблица разделов
   * Теперь для каждого раздела нужно создать свое устройство
   * в devfs
   */

  for (i = 0; i < 4; i++)
  {
    // Тип раздела 0 означает, что разделов больше нет
    if (buf.parts[i].type == 0)
      break;
/*
    kprintf("Раздел #%d, тип 0x%02x%s:\n", i, 
           buf.parts[i].type,
           buf.parts[i].active == 0 ? "" : ", активный");
    kprintf("\tНачало: %d, размер %d\n", 
           buf.parts[i].first_sector,
           buf.parts[i].num_sectors
          );
*/
    int l = strlen(node->name);
    if (l < MAXNAMLEN)
    {
      vfs_node_t *v = vfs_alloc_node();

      strncpy(v->name, node->name, l);

      v->name[l] = i + '0';
      v->name[l+1] = 0;

      v->rights = 660;
      v->gid = v->uid = 0;
      v->flags = VFS_CHARDEV|VFS_STORAGE;

      v->inode = 0;

      v->reserved = buf.parts[i].first_sector*MDPART_BLKSZ;
      v->length = buf.parts[i].num_sectors*MDPART_BLKSZ;

      v->drv = &mdpart_drv;

      v->ptr = node;
      devfs_add(v);
    }
  }

  return 0;
}


uint64_t mdpart_open(vfs_node_t *node, uint32_t flags)
{
  return 0;
}
uint64_t mdpart_close(vfs_node_t *node)
{
  return 0;
}

uint64_t mdpart_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if ((node != NULL) &&
      (node->ptr != NULL))
  {
    if (offset > node->length)
      return EINVAL;

    int len = offset+size > node->length ? node->length - offset : size;

    //kprintf("mdpart_read : offset %d, len %d\n", offset+node->reserved, node->ptr->length);
    return vfs_read(node->ptr, offset+node->reserved, len, buffer);
  } else
    return EBADF;
}

uint64_t mdpart_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if ((node != NULL) &&
      (node->ptr != NULL))
  {
    if (offset > node->length)
      return EINVAL;

    int len = offset+size > node->length ? node->length - offset : size;

    return vfs_write(node->ptr, offset+node->reserved, len, buffer);
  } else
    return EBADF;
}

