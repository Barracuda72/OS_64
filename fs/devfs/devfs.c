/*
 * DEVice File System
 * Файловая система, представляющая устройства
 * Обычно монтируется на /dev
 */

#include <devfs.h>
#include <errno.h>
#include <dirent.h>
#include <mdpart.h>
#include <ext2.h>

#define MAXNODES 100

static vfs_node_t *nodes[MAXNODES];
static int nod_cnt;

vfs_driver_t devfs_drv = {
  NULL,
  NULL,
  NULL,
  NULL,
  devfs_readdir,
  devfs_finddir
};

vfs_node_t *devfs_init()
{
  nod_cnt = 0;

  vfs_node_t *v = vfs_alloc_node();

  strncpy(v->name, "devfs", 5);
  v->name[5] = 0;

  v->rights = 770;
  v->gid = v->uid = 0;
  v->flags = VFS_DIRECTORY;

  v->inode = 0;

  v->reserved = 0;
  v->length = 0;

  v->drv = &devfs_drv;

  v->ptr = NULL;
  return v;
}

struct dirent *devfs_readdir(vfs_node_t *node, uint64_t index)
{
  if ((node != NULL) &&
      (index < nod_cnt))
  {
    struct dirent *d = vfs_alloc_dirent();
    strncpy(d->name, nodes[index]->name, MAXNAMLEN);
    d->inode = nodes[index]->inode;
    return d;
  } else
    return EINVAL;
}

vfs_node_t *devfs_finddir(vfs_node_t *node, char *name)
{
  int i;

  for (i = 0; i < nod_cnt; i++)
  {
    if (strncmp(name, nodes[i]->name, MAXNAMLEN) == 0)
      return nodes[i];
  }

  return ENOENT;
}

int devfs_add(vfs_node_t *node)
{
  if (nod_cnt < MAXNODES)
  {
    nodes[nod_cnt] = node;
    nod_cnt++;
    if (node->flags&VFS_STORAGE)
    {
      // Проверим наличие на устройстве таблицы разделов
      if (mdpart_init(node) == 0)
        return 0;

      // Таблицы разделов нет, проверим ФС
      // EXT2
      if (ext2_init(node) == 0)
        return 0;
      // FAT32
      if (fat32_init(node) == 0)
        return 0;
      // Не нашли ничего
      return 0;
    }
  } else
    return ENFILE;
}

