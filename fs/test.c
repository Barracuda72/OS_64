/*
 * Это - площадка для проверки драйвера файловой системы
 * в hosted-режиме
 */

#include <vfs.h>
#include <initrd.h>
#include <devfs.h>
#include <klibc.h>
#include <mem.h>
#include <errno.h>

void fs_test_main(void *p, int len)
{
  vfs_node_t *ird = initrd_init(p, len);

  vfs_node_t *dfs = devfs_init();

  devfs_add(ird);

  // Распечатаем содержимое devfs
  int i = 0;
  struct dirent *node = 0;
  while ( (node = vfs_readdir(dfs, i)) != EINVAL)
  {
    printf("Найден файл %s\n", node->name);
    vfs_node_t *fsnode = vfs_finddir(dfs, node->name);

    if (fsnode->flags&VFS_DIRECTORY)
      printf("\t(каталог)\n");
#if 1
    else if (strncmp(node->name, "initrd", 7))
    {
      printf("\n\t содержимое: \"");
      char buf[256];
      int sz = vfs_read(fsnode, 0, 256, buf);
      if (sz < 0)
        printf("Что-то не так, код возврата %d\n", sz);
      else {
        int j;
        for (j = 0; j < sz; j++)
          putchar(buf[j]);

        printf("\"\n");
      }
    }
#endif
    i++;
  } 

  kfree(dfs);
  kfree(ird);
}

