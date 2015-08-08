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

#ifndef __HOSTED__
#include <ata.h>
#include <debug.h>
#endif

void fs_print_dir(vfs_node_t *dir)
{
  int i = 0;
  struct dirent *de = NULL;
  //BREAK();
  while ((de = vfs_readdir(dir, i)) != EINVAL)
  {
    if (de != EBADF)
    {
      printf ("Found file %s\n", de->name);
      kfree(de);
    }
    i++;
  }
  printf("---\n");
}

void fs_test_main(void *p, int len)
{
  printf("FS test start...\n");
  vfs_node_t *ird;
  char *rnn;

#ifdef __HOSTED__
  ird = initrd_init(p, len);
  rnn = "initrd0";
#else
  ird = ata_init();
  rnn = "ata0";
#endif

  vfs_node_t *dfs = devfs_init();

  devfs_add(ird);

  vfs_node_t *root = vfs_alloc_node();
  root->name[0] = '/';
  root->name[1] = 0;

  root->flags = VFS_DIRECTORY;

  int res;

  // Распечатаем содержимое devfs
  int i = 0;
  struct dirent *node = 0;
  vfs_node_t *fsnode;
  while ( (node = vfs_readdir(dfs, i)) != EINVAL)
  {
    printf("Found file %s\n", node->name);
    fsnode = vfs_finddir(dfs, node->name);

    if (fsnode->flags&VFS_DIRECTORY)
      printf("\t(directory)\n");

    if (!strncmp(node->name, rnn, 8))
    {
      res = vfs_mount(fsnode, root);
      printf ("Mount : %d\n", res);
    }
#if 0
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
    kfree(node);
  } 

  if (root->ptr != NULL)
  {
    printf("Root mounted successfully\n");
    printf("Contents:\n");
    fs_print_dir(root);

    printf("Contents of hello.txt:\n");
    vfs_node_t *hello = vfs_finddir(root, "hello.txt");
    if (hello != NULL)
    {
      char buf[256];
      int sz = vfs_read(hello, 0, 256, buf);
      if (sz < 0)
        printf("Something wrong, return code %d\n", sz);
      else {
        int j;
        for (j = 0; j < sz; j++)
          printf("%c", buf[j]);

        printf("\"\n");
      }
    } else {
      printf("File not found!\n");
    }
  }

  vfs_umount(root);

  kfree(fsnode);
  kfree(root);
  kfree(dfs);
  kfree(ird);
}

